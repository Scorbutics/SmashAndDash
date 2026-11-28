#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include "Files.h"

namespace {
	using namespace worldgen;
	namespace fs = std::filesystem;

	constexpr std::array<std::uint8_t, 3> Empty{ 255, 255, 255 };
	const char* EventsHeader = "x y declenchement(1:presser espace||2:sous le hero||0:automatique au chargement de la map) parametresSupplementaires";

	// The wild Pokemon of a biome (SPAWNS) : id, level min, level max, path type (1 wanders, 2 stays), chance %
	struct Spawn { int id, levelMin, levelMax, path, chance; };
	const std::vector<Spawn>& SpawnsOf(Biome biome) {
		static const std::array<std::vector<Spawn>, BiomeCount> spawns{ {
			{ { 16, 2, 5, 2, 100 }, { 19, 2, 4, 1, 75 }, { 25, 2, 4, 1, 35 }, { 1, 2, 4, 1, 15 } },
			{ { 10, 3, 6, 1, 80 }, { 16, 3, 6, 2, 100 }, { 1, 3, 5, 1, 30 }, { 19, 3, 5, 1, 40 } },
			{ { 7, 4, 7, 1, 60 }, { 19, 4, 6, 1, 50 }, { 25, 4, 7, 1, 25 } },
			{ { 4, 4, 7, 1, 60 }, { 19, 4, 6, 1, 60 }, { 16, 4, 6, 2, 50 } },
		} };
		return spawns[static_cast<std::size_t>(biome)];
	}

	// The chunks mostly sea (the infinite world : SPAWNS "ocean") : the swimmers, the crabs and Carapuce out of the water,
	// the birds
	const std::vector<Spawn>& OceanSpawns() {
		static const std::vector<Spawn> spawns{ { 72, 6, 10, 1, 70 }, { 129, 4, 9, 1, 80 }, { 120, 7, 11, 1, 35 }, { 98, 6, 10, 1, 45 },
			{ 7, 5, 9, 1, 25 }, { 16, 4, 8, 2, 40 } };
		return spawns;
	}

	void WriteText(const fs::path& path, const std::string& text) {
		auto file = std::ofstream{ path, std::ios::binary | std::ios::trunc };
		if (!file) {
			throw std::runtime_error{ "worldgen : cannot write " + path.string() };
		}
		file << text;
	}

	std::string MapIniText(const std::string& chipset) {
		return "[Chipset]\nfile=" + chipset + "\n\n[Bgm]\nfile=./Music/Greencity.mp3\nvolume=1\n";
	}

	std::string EventsText(const std::vector<std::string>& lines) {
		auto text = std::string{ EventsHeader };
		for (const auto& line : lines) {
			text += "\n" + line;
		}
		return text + "\n";
	}

	// The chipset files : its image, its collisions, its properties (the depth of the tiles), its touch scripts (the
	// water surfs)
	void SaveChipset(const Chipset& chip, const fs::path& chips, const std::string& name, const std::string& tileScripts) {
		const auto base = chips / name;
		WriteFile((chips / (name + ".png")).string(), EncodePng(chip.image));
		WriteFile((chips / (name + ".png.col")).string(), EncodePng(chip.col));
		WriteFile((chips / (name + ".png.mask")).string(), EncodePng(chip.mask));
		WriteFile((chips / (name + ".png.prop")).string(), EncodePng(chip.prop));
		fs::create_directories(base / "Scripts");
		auto list = std::string{};
		for (const auto& [x, y] : chip.water) {
			list += std::to_string(x) + "_" + std::to_string(y) + "\n";
		}
		WriteText(base / "scripts.txt", list);
		for (const auto& [x, y] : chip.water) {
			fs::copy_file(fs::path{ tileScripts } / "water.miniska", base / "Scripts" / (std::to_string(x) + "_" + std::to_string(y) + "_4.miniska"),
				fs::copy_options::overwrite_existing);
		}
	}

	void PutColour(RgbImage& image, int x, int y, const std::uint8_t* colour) {
		std::copy(colour, colour + 3, image.at(x, y));
	}
}

worldgen::RgbImage worldgen::RoomLight(const Room& room) {
	const auto w = room.width;
	const auto h = room.height;
	auto out = RgbImage{ w + 1, h + 1 };
	for (auto j = 0; j <= h; j++) {
		for (auto i = 0; i <= w; i++) {
			auto blocked = 0;
			for (const auto dx : { -1, 0 }) {
				for (const auto dy : { -1, 0 }) {
					const auto x = i + dx;
					const auto y = j + dy;
					blocked += !(0 <= x && x < w && 0 <= y && y < h) || !room.walk[static_cast<std::size_t>(y * w + x)] ? 1 : 0;
				}
			}
			const auto nearWall = std::max(0, 4 - j) / 4.0; // the back wall shades the floor in front of it
			auto l = 0.99 - 0.035 * blocked - 0.05 * nearWall;
			l = std::max(0.85, std::min(1.0, l));
			auto* p = out.at(i, j);
			p[0] = static_cast<std::uint8_t>(static_cast<int>(255 * l * 0.98));
			p[1] = static_cast<std::uint8_t>(static_cast<int>(255 * l * 0.99));
			p[2] = static_cast<std::uint8_t>(static_cast<int>(255 * l));
		}
	}
	return out;
}

namespace {
	void PrepareFolder(const fs::path& root) {
		if (fs::exists(root)) { // an earlier run
			fs::remove_all(root);
		}
		fs::create_directories(root / "Chipsets");
	}

	void WriteChunk(const World& world, TileImages& images, const RgbImage& corr, const std::string& tileScripts, const fs::path& root,
		const std::string& worldPath, int cx, int cy) {
		const auto chips = root / "Chipsets";
		const auto chipPath = worldPath + "Chipsets";
		const auto& plan = world.terrain().plan();
		const auto colour = [&corr](const Block& cell) { return corr.at(cell.first, cell.second); };
		const auto name = "w_" + std::to_string(cx) + "_" + std::to_string(cy);
		const auto map = root / name;
		if (fs::exists(map)) { // a region written again
			fs::remove_all(map);
		}
		fs::create_directories(map / "Monsters");
		const auto chip = Chipset{ images, ChunkTiles(world, cx, cy) };
		SaveChipset(chip, chips, "chipset_" + name, tileScripts);
		const std::pair<Layer, const char*> layers[] = { { Layer::Base, "" }, { Layer::Middle, "M" }, { Layer::Top, "T" } };
		for (const auto& [layer, suffix] : layers) {
			auto image = RgbImage{ Chunk, Chunk, Empty[0], Empty[1], Empty[2] };
			for (auto y = 0; y < Chunk; y++) {
				for (auto x = 0; x < Chunk; x++) {
					const auto wx = cx * Chunk + x;
					const auto wy = cy * Chunk + y;
					const auto tile = world.tile(layer, wx, wy);
					if (tile != NoTile) {
						PutColour(image, x, y, colour(chip.cells.at({ tile, BlockMix(plan, wx, wy) })));
					}
				}
			}
			WriteFile((map / (name + suffix + ".bmp")).string(), EncodeBmp(image));
		}
		auto light = RgbImage{ Chunk + 1, Chunk + 1 };
		for (auto j = 0; j <= Chunk; j++) {
			for (auto i = 0; i <= Chunk; i++) {
				const auto c = world.cornerLight(cx * Chunk + i, cy * Chunk + j);
				PutColour(light, i, j, c.data());
			}
		}
		WriteFile((map / (name + "L.png")).string(), EncodePng(light));
		WriteText(map / (name + ".ini"), MapIniText(chipPath + "/chipset_" + name + ".png"));
		WriteText(map / (name + "E.txt"), EventsText(world.chunkEvents(cx, cy)));
		const auto& spawns = plan.seaChunk(cx, cy) ? OceanSpawns() : SpawnsOf(plan.chunkBiome(cx, cy));
		for (auto i = 0u; i < spawns.size(); i++) {
			const auto& s = spawns[i];
			WriteText(map / "Monsters" / (std::to_string(i) + ".ini"), "[Data]\nid=" + std::to_string(s.id) + "\nlevel_min=" + std::to_string(s.levelMin)
				+ "\nlevel_max=" + std::to_string(s.levelMax) + "\npath_type=" + std::to_string(s.path) + "\n\n[Spawn]\nprobability_percent="
				+ std::to_string(s.chance) + "\n");
		}
		WriteText(map / "scripts.data", "");
	}

	// The rooms : one chipset for all (chipset_rooms ; a region of the infinite world : chipset_rooms_<rx>_<ry>)
	void WriteRooms(const World& world, TileImages& images, const RgbImage& corr, const std::string& tileScripts, const fs::path& root,
		const std::string& worldPath, const std::string& chipset = "chipset_rooms") {
		const auto chipPath = worldPath + "Chipsets";
		const auto colour = [&corr](const Block& cell) { return corr.at(cell.first, cell.second); };
		const auto roomChip = Chipset{ images, RoomTiles(world) };
		SaveChipset(roomChip, root / "Chipsets", chipset, tileScripts);
		const auto meadow = PureMix(Biome::Meadow);
		for (const auto& room : world.rooms()) {
			const auto map = root / room.name;
			if (fs::exists(map)) {
				fs::remove_all(map);
			}
			fs::create_directories(map);
			const std::pair<const std::vector<TileId>*, const char*> layers[] = { { &room.base, "" }, { &room.middle, "M" }, { nullptr, "T" } };
			for (const auto& [grid, suffix] : layers) {
				auto image = RgbImage{ room.width, room.height, Empty[0], Empty[1], Empty[2] };
				if (grid != nullptr) {
					for (auto y = 0; y < room.height; y++) {
						for (auto x = 0; x < room.width; x++) {
							const auto tile = (*grid)[static_cast<std::size_t>(y * room.width + x)];
							if (tile != NoTile) {
								PutColour(image, x, y, colour(roomChip.cells.at({ tile, meadow })));
							}
						}
					}
				}
				WriteFile((map / (room.name + suffix + ".bmp")).string(), EncodeBmp(image));
			}
			WriteFile((map / (room.name + "L.png")).string(), EncodePng(RoomLight(room)));
			WriteText(map / (room.name + ".ini"), MapIniText(chipPath + "/" + chipset + ".png"));
			WriteText(map / (room.name + "E.txt"), EventsText(room.events));
			WriteText(map / "scripts.data", "");
		}
	}
}

void worldgen::WriteWorld(const World& world, TileImages& images, const RgbImage& corr, const std::string& tileScripts,
	const std::string& folder, const std::string& worldPath) {
	const auto root = fs::path{ folder };
	PrepareFolder(root);
	for (const auto& [cx, cy] : world.chunks()) {
		WriteChunk(world, images, corr, tileScripts, root, worldPath, cx, cy);
	}
	WriteRooms(world, images, corr, tileScripts, root, worldPath);
}

void worldgen::WriteRegionChunk(const World& world, TileImages& images, const RgbImage& corr, const std::string& tileScripts,
	const std::string& folder, const std::string& worldPath, int cx, int cy) {
	fs::create_directories(fs::path{ folder } / "Chipsets");
	WriteChunk(world, images, corr, tileScripts, folder, worldPath, cx, cy);
}

void worldgen::WriteRegionRooms(const World& world, TileImages& images, const RgbImage& corr, const std::string& tileScripts,
	const std::string& folder, const std::string& worldPath, const std::string& chipset) {
	fs::create_directories(fs::path{ folder } / "Chipsets");
	WriteRooms(world, images, corr, tileScripts, folder, worldPath, chipset);
}

worldgen::Start worldgen::StartOf(const WorldPlan& plan) {
	const auto [tx, ty] = plan.town();
	const auto cx = tx / Chunk;
	const auto cy = ty / Chunk;
	return { "w_" + std::to_string(cx) + "_" + std::to_string(cy), tx - cx * Chunk, ty - cy * Chunk + 1 };
}

// The steps : the data, the plan (climate, villages), the terrain, the tiles of the blocks, a step per chunk, the rooms
struct worldgen::WorldWriter::State {
	Shapes shapes;
	std::vector<RoomTemplate> templates;
	std::unique_ptr<Atlas> atlas;
	RgbImage corr;
	std::unique_ptr<WorldPlan> plan;
	std::unique_ptr<Terrain> terrain;
	std::unique_ptr<World> world;
	std::unique_ptr<TileImages> images;
};

namespace {
	constexpr int ChunksX = 8;
	constexpr int ChunksY = 8;
	constexpr int FirstChunkStep = 4;
}

worldgen::WorldWriter::WorldWriter(std::int64_t seed, std::string folder, std::string worldPath, Resources resources) :
	m_seed(seed), m_folder(std::move(folder)), m_worldPath(std::move(worldPath)), m_resources(std::move(resources)), m_state(std::make_unique<State>()) {
}

worldgen::WorldWriter::~WorldWriter() = default;

int worldgen::WorldWriter::steps() const {
	return FirstChunkStep + ChunksX * ChunksY + 1;
}

std::string worldgen::WorldWriter::stage() const {
	if (m_step == 0) {
		return "Préparation";
	}
	if (m_step == 1) {
		return "Le climat et les villages";
	}
	if (m_step == 2) {
		return "Le relief et les routes";
	}
	if (m_step == 3) {
		return "La nature et les maisons";
	}
	if (m_step < FirstChunkStep + ChunksX * ChunksY) {
		return "Les cartes (" + std::to_string(m_step - FirstChunkStep + 1) + " / " + std::to_string(ChunksX * ChunksY) + ")";
	}
	return done() ? "Terminé" : "Les intérieurs";
}

bool worldgen::WorldWriter::step() {
	if (done()) {
		return false;
	}
	auto& s = *m_state;
	const auto data = m_resources.root + "/Worldgen";
	const auto tileScripts = m_resources.root + "/Chipsets/tile_scripts";
	if (m_step == 0) {
		auto shapesFile = std::ifstream{ data + "/shapes.txt" };
		auto index = std::ifstream{ data + "/tiles.txt" };
		if (!shapesFile || !index) {
			throw std::runtime_error{ "worldgen : no " + data + "/shapes.txt or tiles.txt" };
		}
		s.shapes = Shapes::Parse(shapesFile);
		s.templates = LoadRoomTemplates(data + "/rooms");
		// The painted tiles (tools/render3d), when there are
		auto paintedIndex = std::ifstream{ data + "/painted.txt" };
		if (paintedIndex.good()) {
			s.atlas = std::make_unique<Atlas>(ReadPng(data + "/tiles.png"), index, ReadPng(data + "/painted.png"), paintedIndex);
		} else {
			s.atlas = std::make_unique<Atlas>(ReadPng(data + "/tiles.png"), index);
		}
		s.corr = ReadPng(m_resources.root + "/Chipsets/corr.png");
		PrepareFolder(m_folder);
	} else if (m_step == 1) {
		s.plan = std::make_unique<WorldPlan>(m_seed, ChunksX, ChunksY);
		m_start = StartOf(*s.plan);
	} else if (m_step == 2) {
		s.terrain = std::make_unique<Terrain>(*s.plan);
	} else if (m_step == 3) {
		s.world = std::make_unique<World>(*s.terrain, s.shapes, s.templates);
		s.images = std::make_unique<TileImages>(*s.atlas, s.world->tiles());
	} else if (m_step < FirstChunkStep + ChunksX * ChunksY) {
		const auto chunk = m_step - FirstChunkStep;
		WriteChunk(*s.world, *s.images, s.corr, tileScripts, m_folder, m_worldPath, chunk % ChunksX, chunk / ChunksX);
	} else {
		WriteRooms(*s.world, *s.images, s.corr, tileScripts, m_folder, m_worldPath);
		m_state.reset();
	}
	m_step++;
	return !done();
}

worldgen::Start worldgen::GenerateWorld(std::int64_t seed, const std::string& folder, const std::string& worldPath, const Resources& resources) {
	auto writer = WorldWriter{ seed, folder, worldPath, resources };
	while (writer.step()) {
	}
	return writer.start();
}
