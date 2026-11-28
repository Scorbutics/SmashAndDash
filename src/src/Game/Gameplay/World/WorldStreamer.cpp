#include <algorithm>
#include <chrono>
#include <fstream>
#include "Game/__internalConfig/LoggerConfig.h"
#include "WorldStreamer.h"
#include "WorldImpl.h"
#include "ChunkEntities.h"
#include "WorldLoader.h"
#include "../Data/SavePaths.h"
#include "../../Utils/IDs.h"
#include "Base/IO/Readers/IniReader.h"
#include "Core/Exceptions/CorruptedFileException.h"
#include "Core/Exceptions/ExceptionTrigger.h"
#include "Core/Utils/TimeUtils.h"
#include "Core/Draw/DrawableContainer.h"
#include "Graphic/Texture.h"
#include "Graphic/SDLSurface.h"
#include "World/TilesetLoaderImage.h"
#include "World/TilesetEventLoaderText.h"
#include "World/TileWorldPhysics.h"
#include "Physic/Space.h"
#include "Chunks/ChunkGenerator.h"
#include "Generation/InfiniteFiles.h"

namespace {
	// Chunks being prepared at once
	constexpr auto PendingMax = 2;
	// An infinite world : its chunks written ahead of the player, within this many blocks (on a thread ; else only the
	// ones to load)
	constexpr auto GenerateRadius = chunks::LoadRadius + 60;

	ska::SDLSurface Surface(const worldgen::RgbImage& image, const ska::Color* key = nullptr) {
		auto surface = ska::SDLSurface{};
		if (image.width > 0 && image.height > 0) {
			surface.loadFromRgb(image.rgb.data(), image.width, image.height, key);
		}
		return surface;
	}

	// The size (blocks) of a map : of its ground layer
	ska::Point<int> MapSize(const std::string& name) {
		auto file = std::ifstream{ MapDirectory(name) + "/" + name + ".bmp", std::ios::binary };
		unsigned char header[26] = {};
		if (!file.read(reinterpret_cast<char*>(header), sizeof(header))) {
			ska::ExceptionTrigger<ska::CorruptedFileException>("No map " + name + " in " + WorldDirectory());
		}
		const auto number = [&header](int at) {
			return static_cast<int>(header[at] | (header[at + 1] << 8) | (header[at + 2] << 16) | (static_cast<unsigned int>(header[at + 3]) << 24));
		};
		const auto height = number(22);
		return { number(18), height < 0 ? -height : height };
	}

	bool IsMap(const std::string& name) {
		return std::ifstream{ MapIni(name) }.good();
	}

	chunks::Grid GridOf(const std::string& map) {
		const auto chunk = chunks::Parse(map);
		// The outdoors of an infinite world : WorldChunks x WorldChunks chunks, written as the player comes near them
		if (chunk && worldgen::InfiniteSeed(WorldDirectory())) {
			return chunks::Grid{ worldgen::WorldChunks, worldgen::WorldChunks, worldgen::Chunk, worldgen::Chunk, {} };
		}
		const auto size = MapSize(map);
		auto grid = chunks::Grid{ 1, 1, size.x, size.y, {} };
		if (!chunk) {
			grid.single = map;
			return grid;
		}
		// The outdoors : the chunks there are
		grid.width = chunk->x + 1;
		grid.height = chunk->y + 1;
		while (IsMap("w_" + std::to_string(grid.width) + "_0")) {
			grid.width++;
		}
		while (IsMap("w_0_" + std::to_string(grid.height))) {
			grid.height++;
		}
		return grid;
	}
}

WorldStreamer::WorldStreamer(WorldImpl& world, const ska::TilesetCorrespondanceMapper& mapper, ChunkEntities& entities) :
	m_world(world),
	m_mapper(mapper),
	m_entities(entities),
	// The fixed step runs (tools/sad-run --fixed-step) : the same chunks at the same steps every time
	m_queue(chunks::MakePrepareQueue(ska::TimeUtils::fixedStep())) {
}

WorldStreamer::~WorldStreamer() = default;

const chunks::Paths& WorldStreamer::pathsOf(const chunks::Coordinates c) {
	if (const auto found = m_paths.find(c); found != m_paths.end()) {
		return found->second;
	}
	const auto name = m_grid.name(c);
	const auto ini = ska::IniReader{ MapIni(name) };
	const auto chipset = TilesetName(ini.get<std::string>("Chipset file"));
	return m_paths.emplace(c, chunks::PathsOf(MapDirectory(name), name, chipset + ".png")).first->second;
}

ska::Rectangle WorldStreamer::blocksOf(const chunks::Coordinates c) const {
	return { m_grid.originX(c), m_grid.originY(c), m_grid.chunkWidth, m_grid.chunkHeight };
}

ska::Point<int> WorldStreamer::originOf(const std::string& map) const {
	const auto c = chunks::Parse(map);
	if (!m_grid.single.empty() || !c || !m_grid.contains(*c)) {
		return { 0, 0 };
	}
	return { m_grid.originX(*c), m_grid.originY(*c) };
}

const ska::Tileset& WorldStreamer::tilesetAt(const ska::Point<int> block) const {
	auto found = m_chunks.find(m_grid.at(block.x, block.y));
	if (found == m_chunks.end() && m_current) {
		found = m_chunks.find(*m_current);
	}
	if (found == m_chunks.end()) {
		found = m_chunks.begin();
	}
	assert(found != m_chunks.end() && "No chunk loaded");
	return *found->second->tileset;
}

std::vector<std::pair<std::string, ska::Rectangle>> WorldStreamer::loaded() const {
	auto result = std::vector<std::pair<std::string, ska::Rectangle>>{};
	for (const auto& [c, chunk] : m_chunks) {
		result.emplace_back(chunk->name, blocksOf(c));
	}
	return result;
}

ska::Point<int> WorldStreamer::enter(const std::string& map, const ska::Point<int> localBlock, ska::cp::Space& space) {
	m_space = &space;
	m_grid = GridOf(map);
	openGenerator();
	m_paths.clear();
	m_current = std::nullopt;
	m_world.reset(WorldDirectory() + (m_grid.single.empty() ? "outdoors" : m_grid.single), m_grid.width, m_grid.height, m_grid.chunkWidth, m_grid.chunkHeight, 3);
	const auto origin = originOf(map);
	const auto block = ska::Point<int>{ origin.x + localBlock.x, origin.y + localBlock.y };
	for (const auto& c : m_grid.around(block.x, block.y, chunks::LoadRadius)) {
		loadNow(c);
	}
	settings(block);
	return block;
}

void WorldStreamer::leave() {
	while (!m_chunks.empty()) {
		unload(m_chunks.begin()->first);
	}
	// What is being prepared is for this world : thrown away when it comes
	m_pending.clear();
	m_ready.clear();
	m_staged = std::nullopt;
}

void WorldStreamer::update(const ska::Point<int> block) {
	while (auto prepared = m_queue->poll()) {
		const auto found = m_pending.find(prepared->id);
		if (found != m_pending.end()) {
			m_ready.emplace(found->second, std::move(prepared->images));
			m_pending.erase(found);
		}
	}

	// Gone too far
	auto leaving = std::vector<chunks::Coordinates>{};
	for (const auto& [c, chunk] : m_chunks) {
		if (m_grid.distance(c, block.x, block.y) > chunks::UnloadRadius) {
			leaving.push_back(c);
		}
	}
	for (const auto& c : leaving) {
		unload(c);
	}
	for (auto it = m_ready.begin(); it != m_ready.end();) {
		it = m_grid.distance(it->first, block.x, block.y) > chunks::UnloadRadius ? m_ready.erase(it) : std::next(it);
	}

	// The chunk begun at the step before is finished
	auto busy = false;
	if (m_staged) {
		auto staged = std::move(*m_staged);
		m_staged = std::nullopt;
		if (m_grid.distance(staged.at, block.x, block.y) <= chunks::UnloadRadius) {
			commit(std::move(staged));
			busy = true;
		}
	}
	// An infinite world : the chunks to write, the nearest first
	if (generating()) {
		auto wishes = std::vector<chunks::Coordinates>{};
		const auto radius = m_generator->synchronous() ? chunks::LoadRadius : GenerateRadius;
		for (const auto& c : m_grid.around(block.x, block.y, radius)) {
			if (m_chunks.count(c) == 0 && !m_generator->written(c)) {
				wishes.push_back(c);
			}
		}
		m_generator->wish(std::move(wishes));
		m_generator->step();
	}
	// To come, the nearest first
	for (const auto& c : m_grid.around(block.x, block.y, chunks::LoadRadius)) {
		if (m_chunks.count(c) != 0) {
			continue;
		}
		// (an infinite world : once written, but the ones the player would see missing)
		if (m_grid.distance(c, block.x, block.y) > chunks::VisibleRadius && !ready(c)) {
			continue;
		}
		if (m_grid.distance(c, block.x, block.y) <= chunks::VisibleRadius) {
			// The player would see it missing : now
			loadNow(c);
			busy = true;
			continue;
		}
		const auto ready = m_ready.find(c);
		if (ready != m_ready.end()) {
			// One part of one chunk a step
			if (!busy && !m_staged) {
				auto images = std::move(ready->second);
				m_ready.erase(ready);
				m_staged = stage(c, std::move(images));
				busy = true;
			}
			continue;
		}
		const auto pending = std::any_of(m_pending.begin(), m_pending.end(), [&c](const auto& p) { return p.second == c; });
		const auto staged = m_staged && m_staged->at == c;
		if (!pending && !staged && m_pending.size() < PendingMax) {
			const auto id = ++m_lastRequest;
			m_pending.emplace(id, c);
			m_queue->request(id, pathsOf(c));
		}
	}
	settings(block);
}

void WorldStreamer::openGenerator() {
	const auto seed = m_grid.single.empty() ? worldgen::InfiniteSeed(WorldDirectory()) : std::nullopt;
	if (!seed) {
		return;
	}
	if (m_generator != nullptr && m_generator->folder() == WorldDirectory()) {
		return;
	}
	auto writer = chunks::TakeOver(WorldDirectory());
	if (writer == nullptr) {
		writer = std::make_unique<worldgen::InfiniteWriter>(*seed, WorldDirectory(), WorldDirectory());
	}
	m_generator = std::make_unique<chunks::Generator>(std::move(writer), ska::TimeUtils::fixedStep());
	SLOG(ska::LogLevel::Info) << "Chunks : the infinite world of seed " << *seed << " (" << WorldDirectory() << ")"
		<< (m_generator->synchronous() ? ", written in the steps of the game" : ", written on a thread");
}

bool WorldStreamer::ready(const chunks::Coordinates c) {
	if (!generating()) {
		return true;
	}
	return m_generator->written(c);
}

void WorldStreamer::loadNow(const chunks::Coordinates c) {
	if (generating() && !m_generator->written(c)) {
		const auto begin = std::chrono::steady_clock::now();
		m_generator->now(c);
		SLOG(ska::LogLevel::Info) << "Chunk " << m_grid.name(c) << " written at once in "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() << " ms";
	}
	if (m_staged && m_staged->at == c) {
		auto staged = std::move(*m_staged);
		m_staged = std::nullopt;
		commit(std::move(staged));
		return;
	}
	for (auto it = m_pending.begin(); it != m_pending.end();) {
		it = it->second == c ? m_pending.erase(it) : std::next(it);
	}
	const auto ready = m_ready.find(c);
	if (ready != m_ready.end()) {
		auto images = std::move(ready->second);
		m_ready.erase(ready);
		commit(stage(c, std::move(images)));
		return;
	}
	commit(stage(c, chunks::Decode(chunks::Read(pathsOf(c)))));
}

WorldStreamer::Staged WorldStreamer::stage(const chunks::Coordinates c, chunks::Images images) {
	const auto name = m_grid.name(c);
	if (!images.error.empty()) {
		ska::ExceptionTrigger<ska::CorruptedFileException>("Chunk " + name + " : " + images.error);
	}
	const auto tilesetName = TilesetName(pathsOf(c)[chunks::Chipset]);

	const auto key = ska::Color{ ska::Texture::DEFAULT_T_RED, ska::Texture::DEFAULT_T_GREEN, ska::Texture::DEFAULT_T_BLUE, 255 };
	auto chunk = std::make_unique<Chunk>();
	chunk->name = name;
	const auto tilesetLoader = ska::TilesetLoaderImage{ tilesetName, Surface(images.image[chunks::Chipset], &key), Surface(images.image[chunks::Collisions]),
		Surface(images.image[chunks::Properties]), Surface(images.image[chunks::Mask]) };
	chunk->tileset = std::make_unique<ska::Tileset>(BlockSize, tilesetLoader, ska::TilesetEventLoaderText{ tilesetName });
	MarkWater(*chunk->tileset);
	return { c, std::move(chunk), std::move(images) };
}

void WorldStreamer::commit(Staged staged) {
	const auto begin = std::chrono::steady_clock::now();
	const auto c = staged.at;
	auto chunk = std::move(staged.chunk);
	auto& images = staged.images;
	const auto name = chunk->name;
	const auto directory = MapDirectory(name);
	const auto& paths = pathsOf(c);
	const auto key = ska::Color{ ska::Texture::DEFAULT_T_RED, ska::Texture::DEFAULT_T_GREEN, ska::Texture::DEFAULT_T_BLUE, 255 };

	const auto loader = BuildWorldLoader(m_mapper, directory, Surface(images.image[chunks::Ground]), Surface(images.image[chunks::Middle]), Surface(images.image[chunks::Top]));
	// The loading events of a chunk run the first time only : its entities are kept after that
	m_world.addChunk({ c.x, c.y }, loader, *chunk->tileset, !m_entities.known(name));

	const auto& light = images.image[chunks::Light];
	if (light.width == m_grid.chunkWidth + 1 && light.height == m_grid.chunkHeight + 1) {
		chunk->light.setPriority(LightMapPriority);
		chunk->light.load(paths[chunks::Light], Surface(light, &key), BlockSize, m_grid.chunkWidth, m_grid.chunkHeight,
			{ m_grid.originX(c) * static_cast<int>(BlockSize), m_grid.originY(c) * static_cast<int>(BlockSize) });
	}

	auto& added = *chunk;
	m_chunks[c] = std::move(chunk);
	buildWalls(c, added);
	buildWaterAround(c);
	m_entities.arrive(name, blocksOf(c));
	SLOG_STATIC(ska::LogLevel::Info, WorldStreamer) << "Chunk load " << name << " " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count() << " us";
}

void WorldStreamer::unload(const chunks::Coordinates c) {
	const auto found = m_chunks.find(c);
	if (found == m_chunks.end()) {
		return;
	}
	m_entities.leave(found->second->name, blocksOf(c));
	for (const auto shape : found->second->walls) {
		m_space->removeShape(shape);
	}
	for (const auto shape : found->second->water) {
		m_space->removeShape(shape);
	}
	SLOG_STATIC(ska::LogLevel::Info, WorldStreamer) << "Chunk unload " << found->second->name;
	m_world.removeChunk({ c.x, c.y });
	m_chunks.erase(found);
	buildWaterAround(c);
}

void WorldStreamer::buildWalls(const chunks::Coordinates c, Chunk& chunk) {
	const auto* collisions = m_world.chunkCollisions({ c.x, c.y });
	const auto size = static_cast<int>(BlockSize);
	// The walls are filled (nothing can stand inside a wall, ex : after leaving the water) ; the water is only its outline
	const auto walls = ska::GenerateFilledTileMap(1, *collisions, [](const ska::Tile* b) {
		return b == nullptr || IsWater(*b) ? ska::TileCollision::No : b->collision;
	});
	for (auto r : walls) {
		r.x += m_grid.originX(c) * size;
		r.y += m_grid.originY(c) * size;
		const auto index = m_space->addShape(nullptr, ska::cp::Shape::fromBox(m_space->getStaticBody(), r));
		auto& shape = m_space->getShape(index);
		shape.setBounciness(1.F);
		shape.setFriction(1.F);
		chunk.walls.push_back(index);
	}
}

bool WorldStreamer::water(const int x, const int y) const {
	const auto* block = m_world.getCollisionProfile().getHighestBlockOnPredicate(1, static_cast<std::size_t>(x), static_cast<std::size_t>(y),
		[](const ska::Tile* b) { return b != nullptr && IsWater(*b) ? ska::TileCollision::Yes : ska::TileCollision::No; });
	return block != nullptr && IsWater(*block);
}

void WorldStreamer::buildWater(const chunks::Coordinates c) {
	const auto found = m_chunks.find(c);
	if (found == m_chunks.end()) {
		return;
	}
	auto& chunk = *found->second;
	for (const auto shape : chunk.water) {
		m_space->removeShape(shape);
	}
	chunk.water.clear();
	const auto blocks = blocksOf(c);
	const auto size = static_cast<int>(BlockSize);
	// An edge of a water block, with ground on the other side : out of the world is ground, a chunk not there yet is water
	// (made again when it comes)
	const auto shore = [this](int x, int y) {
		if (x < 0 || y < 0 || x >= m_grid.width * m_grid.chunkWidth || y >= m_grid.height * m_grid.chunkHeight) {
			return true;
		}
		return m_chunks.count(m_grid.at(x, y)) != 0 && !water(x, y);
	};
	const auto add = [this, &chunk](const ska::Rectangle& r) {
		const auto index = m_space->addShape(nullptr, ska::cp::Shape::fromBox(m_space->getStaticBody(), r));
		auto& shape = m_space->getShape(index);
		shape.setBounciness(1.F);
		shape.setFriction(1.F);
		shape.setCollisionType(ChipmunkWaterCollisionType);
		chunk.water.push_back(index);
	};
	// A thin box along each run of edges, its ends in by a pixel (as ska GenerateContourTileMap)
	constexpr auto Depth = 3;
	for (const auto side : { -1, 1 }) {
		// The horizontal edges (above : side -1, below : 1)
		for (auto y = blocks.y; y < blocks.y + blocks.h; y++) {
			auto start = -1;
			for (auto x = blocks.x; x <= blocks.x + blocks.w; x++) {
				const auto edge = x < blocks.x + blocks.w && water(x, y) && shore(x, y + side);
				if (edge && start < 0) {
					start = x;
				} else if (!edge && start >= 0) {
					const auto line = (side < 0 ? y : y + 1) * size;
					add({ start * size + Depth / 2, line - Depth / 2, (x - start) * size - Depth / 2, Depth });
					start = -1;
				}
			}
		}
		// The vertical ones (left : side -1, right : 1)
		for (auto x = blocks.x; x < blocks.x + blocks.w; x++) {
			auto start = -1;
			for (auto y = blocks.y; y <= blocks.y + blocks.h; y++) {
				const auto edge = y < blocks.y + blocks.h && water(x, y) && shore(x + side, y);
				if (edge && start < 0) {
					start = y;
				} else if (!edge && start >= 0) {
					const auto line = (side < 0 ? x : x + 1) * size;
					add({ line - Depth / 2, start * size + Depth / 2, Depth, (y - start) * size - Depth / 2 });
					start = -1;
				}
			}
		}
	}
}

void WorldStreamer::buildWaterAround(const chunks::Coordinates c) {
	for (const auto& [dx, dy] : { std::pair<int, int>{ 0, 0 }, { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } }) {
		buildWater({ c.x + dx, c.y + dy });
	}
}

void WorldStreamer::settings(const ska::Point<int> block) {
	const auto c = m_grid.at(block.x, block.y);
	if (m_current && *m_current == c) {
		return;
	}
	m_current = c;
	m_world.setMap(MapDirectory(m_grid.name(c)));
}

void WorldStreamer::updateAnimations() {
	for (auto& [c, chunk] : m_chunks) {
		chunk->tileset->update();
	}
}

void WorldStreamer::graphicUpdate(const ska::Rectangle& camera, ska::DrawableContainer& drawables) {
	const auto width = m_grid.chunkWidth * static_cast<int>(BlockSize);
	const auto height = m_grid.chunkHeight * static_cast<int>(BlockSize);
	for (auto& [c, chunk] : m_chunks) {
		const auto x = c.x * width;
		const auto y = c.y * height;
		if (x <= camera.x + camera.w && y <= camera.y + camera.h && x + width >= camera.x && y + height >= camera.y) {
			chunk->light.graphicUpdate(camera, drawables);
		}
	}
}
