#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <tuple>
#include "Chipset.h"

namespace {
	using namespace worldgen;

	constexpr std::array<std::uint8_t, 3> Key{ 190, 151, 255 };
	constexpr std::array<std::uint8_t, 3> Walk{ 255, 255, 255 };
	constexpr std::array<std::uint8_t, 3> Blocked{ 0, 0, 0 };
	constexpr std::array<std::uint8_t, 3> WalkAnimated{ 0xAA, 0xAA, 0xAA };
	constexpr std::array<std::uint8_t, 3> BlockedAnimated{ 0x46, 0x46, 0x46 };

	// The game makes these cells water in every chipset (WorldState BuildTileset) : the water goes there, nothing else
	bool InWaterRegion(const Block& c) {
		return (c.first < 6 && c.second < 4) || (c.first < 3 && c.second >= 4 && c.second < 8);
	}

	std::vector<Block> WaterSlots() {
		auto out = std::vector<Block>{ { 1, 4 } };
		for (auto x = 0; x < 6; x++) {
			out.push_back({ x, 0 });
		}
		out.push_back({ 0, 4 });
		out.push_back({ 2, 4 });
		return out;
	}

	// Where the other animated tiles go (their 4 frames down from there)
	std::vector<Block> AnimatedSlots() {
		auto out = std::vector<Block>{};
		for (auto x = 3; x < ChipColumns; x++) {
			out.push_back({ x, 4 });
		}
		for (auto x = 6; x < ChipColumns; x++) {
			out.push_back({ x, 0 });
		}
		for (auto y = 8; y < ChipRows; y += 4) {
			for (auto x = 0; x < ChipColumns; x++) {
				out.push_back({ x, y });
			}
		}
		return out;
	}

	// The weights of the 4 corners (NW NE SW SE, 0 .. 1) blended across a tile, per voxel (blend_mask)
	std::array<std::uint8_t, Voxels * Voxels> BlendMask(const std::array<double, 4>& weights) {
		auto mask = std::array<std::uint8_t, Voxels * Voxels>{};
		const auto [nw, ne, sw, se] = weights;
		for (auto vy = 0; vy < Voxels; vy++) {
			const auto v = (vy + 0.5) / Voxels;
			for (auto vx = 0; vx < Voxels; vx++) {
				const auto u = (vx + 0.5) / Voxels;
				const auto w = (nw * (1 - u) + ne * u) * (1 - v) + (sw * (1 - u) + se * u) * v;
				mask[static_cast<std::size_t>(vy * Voxels + vx)] = static_cast<std::uint8_t>(static_cast<int>(std::nearbyint(255 * w)));
			}
		}
		return mask;
	}
}

worldgen::Mix worldgen::PureMix(Biome biome) {
	auto corner = Corner{};
	corner[static_cast<std::size_t>(biome)] = MixSteps;
	return { corner, corner, corner, corner };
}

worldgen::Mix worldgen::BlockMix(const WorldPlan& plan, int x, int y) {
	return { plan.corner(x, y), plan.corner(x + 1, y), plan.corner(x, y + 1), plan.corner(x + 1, y + 1) };
}

worldgen::TileImage worldgen::TileImage::full() const {
	if (side == TileSize) {
		return *this;
	}
	auto out = TileImage{ TileSize, std::vector<std::uint8_t>(static_cast<std::size_t>(TileSize * TileSize * 3)) };
	const auto scale = TileSize / side;
	for (auto py = 0; py < TileSize; py++) {
		for (auto px = 0; px < TileSize; px++) {
			const auto* v = &rgb[static_cast<std::size_t>(((py / scale) * side + px / scale) * 3)];
			std::copy(v, v + 3, out.rgb.begin() + (py * TileSize + px) * 3);
		}
	}
	return out;
}

worldgen::Atlas::Atlas(RgbImage image, std::istream& index, RgbImage painted, std::istream& paintedIndex) : Atlas(std::move(image), index) {
	m_painted = std::move(painted);
	auto line = std::string{};
	auto last = -1;
	while (std::getline(paintedIndex, line)) {
		if (line.empty()) {
			continue;
		}
		// "<biome> <cell> <key>"
		const auto space = line.find(' ');
		const auto second = line.find(' ', space + 1);
		const auto cell = std::stoi(line.substr(space + 1, second - space - 1));
		m_paintedRows[{ std::stoi(line.substr(0, space)), line.substr(second + 1) }] = cell;
		last = std::max(last, cell);
	}
	const auto columns = m_painted.width / TileSize;
	if (last >= 0 && (columns == 0 || m_painted.height < (last / columns + 1) * TileSize)) {
		throw std::runtime_error{ "worldgen : the painted tiles are smaller than their index" };
	}
}

worldgen::Atlas::Atlas(RgbImage image, std::istream& index) : m_image(std::move(image)) {
	auto line = std::string{};
	auto row = 0;
	while (std::getline(index, line)) {
		if (line.empty()) {
			continue;
		}
		// "<frames> <base> <flags> <mask> <key>"
		const auto space = line.find(' ');
		const auto second = line.find(' ', space + 1);
		const auto third = line.find(' ', second + 1);
		const auto fourth = line.find(' ', third + 1);
		const auto key = line.substr(fourth + 1);
		m_rows[key] = { row++, std::stoi(line.substr(0, space)) };
		m_bases[key] = std::stoi(line.substr(space + 1, second - space - 1));
		m_flags[key] = std::stoi(line.substr(second + 1, third - second - 1));
		m_masks[key] = std::stoi(line.substr(third + 1, fourth - third - 1));
	}
	if (m_image.height < row * Voxels || m_image.width < BiomeCount * 4 * Voxels) {
		throw std::runtime_error{ "worldgen : the atlas of the tiles is smaller than its index" };
	}
}

std::vector<worldgen::TileImage> worldgen::Atlas::frames(const std::string& key, Biome biome) const {
	const auto found = m_rows.find(key);
	if (found == m_rows.end()) {
		throw std::runtime_error{ "worldgen : the tile " + key + " is not in the atlas (tools/worldgen/atlas.py)" };
	}
	const auto [row, count] = found->second;
	auto out = std::vector<TileImage>(static_cast<std::size_t>(count));
	// Painted in this biome : its pixels
	const auto painted = m_paintedRows.find({ static_cast<int>(biome), key });
	if (painted != m_paintedRows.end()) {
		// Its frames in the cells that follow the first one
		const auto columns = m_painted.width / TileSize;
		for (auto f = 0; f < count; f++) {
			auto& image = out[static_cast<std::size_t>(f)];
			image.side = TileSize;
			image.rgb.resize(static_cast<std::size_t>(TileSize * TileSize * 3));
			const auto cell = painted->second + f;
			for (auto py = 0; py < TileSize; py++) {
				const auto* src = m_painted.at((cell % columns) * TileSize, (cell / columns) * TileSize + py);
				std::copy(src, src + TileSize * 3, image.rgb.begin() + py * TileSize * 3);
			}
		}
		return out;
	}
	for (auto f = 0; f < count; f++) {
		auto& image = out[static_cast<std::size_t>(f)];
		image.rgb.resize(static_cast<std::size_t>(Voxels * Voxels * 3));
		const auto x0 = (static_cast<int>(biome) * 4 + f) * Voxels;
		for (auto vy = 0; vy < Voxels; vy++) {
			const auto* src = m_image.at(x0, row * Voxels + vy);
			std::copy(src, src + Voxels * 3, image.rgb.begin() + vy * Voxels * 3);
		}
	}
	return out;
}

const std::vector<worldgen::TileImage>& worldgen::TileImages::pure(TileId tile, Biome biome) {
	auto found = m_pure.find({ tile, biome });
	if (found == m_pure.end()) {
		found = m_pure.emplace(std::make_pair(tile, biome), m_atlas.frames(key(tile), biome)).first;
	}
	return found->second;
}

const std::vector<worldgen::TileImage>& worldgen::TileImages::frames(TileId tile, const Mix& mix) {
	auto found = m_cache.find({ tile, mix });
	if (found != m_cache.end()) {
		return found->second;
	}
	// The geometry of a tile does not change with the colours : a mix is the blend, voxel by voxel, of the tile in
	// each biome (PIL Image.composite : (out * (255 - mask) + in * mask) / 255, rounded as PIL does)
	auto out = std::vector<TileImage>{};
	auto total = std::array<double, 4>{};
	for (auto k = 0; k < BiomeCount; k++) {
		auto w = std::array<double, 4>{};
		auto any = false;
		for (auto i = 0u; i < 4; i++) {
			w[i] = mix[i][static_cast<std::size_t>(k)] / static_cast<double>(MixSteps);
			any = any || w[i] != 0.0;
		}
		if (!any) {
			continue;
		}
		const auto& images = pure(tile, static_cast<Biome>(k));
		if (out.empty()) {
			out = images;
			total = w;
			continue;
		}
		auto ratio = std::array<double, 4>{};
		for (auto i = 0u; i < 4; i++) {
			total[i] = total[i] + w[i];
			ratio[i] = total[i] != 0.0 ? w[i] / total[i] : 0.0;
		}
		const auto mask = BlendMask(ratio);
		for (auto f = 0u; f < out.size(); f++) {
			// A painted tile and a voxel one : both at one pixel per pixel, the weight of a voxel on its 3 x 3 pixels (as
			// Python blends its 48 px drawings)
			auto in = images[f];
			if (in.side != out[f].side) {
				in = in.full();
				out[f] = out[f].full();
			}
			const auto side = out[f].side;
			const auto scale = side / Voxels;
			for (auto py = 0; py < side; py++) {
				for (auto px = 0; px < side; px++) {
					const unsigned m = mask[static_cast<std::size_t>((py / scale) * Voxels + px / scale)];
					const auto p = static_cast<std::size_t>(py * side + px);
					for (auto c = 0u; c < 3; c++) {
						const auto t = static_cast<unsigned>(out[f].rgb[p * 3 + c]) * (255 - m) + static_cast<unsigned>(in.rgb[p * 3 + c]) * m + 128;
						out[f].rgb[p * 3 + c] = static_cast<std::uint8_t>(((t >> 8) + t) >> 8);
					}
				}
			}
		}
	}
	return m_cache.emplace(std::make_pair(tile, mix), std::move(out)).first->second;
}

worldgen::Chipset::Chipset(TileImages& images, const std::set<TileUse>& usedTiles) {
	auto taken = std::set<Block>{};
	for (auto y = 0; y < ChipRows; y++) {
		for (auto x = 0; x < ChipColumns; x++) {
			if (InWaterRegion({ x, y })) {
				taken.insert({ x, y });
			}
		}
	}
	auto grid = std::map<Block, TileImage>{};
	auto colours = std::map<Block, std::array<std::uint8_t, 3>>{};
	auto bases = std::map<Block, std::pair<int, int>>{};
	auto masks = std::map<Block, int>{};
	auto animatedCells = std::map<std::pair<TileId, std::vector<TileImage>>, Block>{};
	auto stillCells = std::map<std::tuple<bool, int, int, int, TileImage>, Block>{};
	auto waterSlots = WaterSlots();
	auto animatedSlots = AnimatedSlots();
	// The order of the Python generator : by the text of the tile, then by the mix
	auto order = std::vector<std::tuple<std::string, Mix, TileId>>{};
	for (const auto& [tile, mix] : usedTiles) {
		order.emplace_back(images.sortName(tile), mix, tile);
	}
	std::sort(order.begin(), order.end());
	// The animated tiles first : 4 cells in a column
	for (const auto& [name, mix, tile] : order) {
		if (!IsAnimated(tile)) {
			continue;
		}
		const auto& frames = images.frames(tile, mix);
		const auto sig = std::make_pair(tile, frames);
		auto found = animatedCells.find(sig);
		if (found == animatedCells.end()) {
			auto slot = Block{};
			if (AnimatedOf(tile) == Animated::Water) {
				if (waterSlots.empty()) {
					throw std::runtime_error{ "worldgen : too many kinds of water in one map" };
				}
				slot = waterSlots.front();
				waterSlots.erase(waterSlots.begin());
				water.push_back(slot);
			} else {
				const auto free = std::find_if(animatedSlots.begin(), animatedSlots.end(), [&taken](const Block& c) {
					for (auto f = 0; f < 4; f++) {
						if (taken.count({ c.first, c.second + f })) {
							return false;
						}
					}
					return true;
				});
				if (free == animatedSlots.end()) {
					throw std::runtime_error{ "worldgen : no room for the animated tiles of a map" };
				}
				slot = *free;
				animatedSlots.erase(free);
			}
			for (auto f = 0u; f < frames.size(); f++) {
				const auto cell = Block{ slot.first, slot.second + static_cast<int>(f) };
				grid[cell] = frames[f];
				taken.insert(cell);
			}
			colours[slot] = images.walkable(tile) ? WalkAnimated : BlockedAnimated;
			for (auto f = 0u; f < frames.size(); f++) {
				masks[{ slot.first, slot.second + static_cast<int>(f) }] = images.mask(tile);
			}
			bases[slot] = { images.base(tile), images.flags(tile) };
			found = animatedCells.emplace(sig, slot).first;
		}
		cells[{ tile, mix }] = found->second;
	}
	auto free = std::vector<Block>{};
	for (auto y = 0; y < ChipRows; y++) {
		for (auto x = 0; x < ChipColumns; x++) {
			if (!taken.count({ x, y })) {
				free.push_back({ x, y });
			}
		}
	}
	auto next = free.begin();
	for (const auto& [name, mix, tile] : order) {
		if (IsAnimated(tile)) {
			continue;
		}
		const auto& frame = images.frames(tile, mix)[0];
		const auto sig = std::make_tuple(images.walkable(tile), images.base(tile), images.flags(tile), images.mask(tile), frame);
		auto found = stillCells.find(sig);
		if (found == stillCells.end()) {
			if (next == free.end()) {
				throw std::runtime_error{ "worldgen : the chipset of a map is full (" + std::to_string(usedTiles.size()) + " tiles)" };
			}
			const auto cell = *next++;
			taken.insert(cell);
			grid[cell] = frame;
			colours[cell] = images.walkable(tile) ? Walk : Blocked;
			masks[cell] = images.mask(tile);
			bases[cell] = { images.base(tile), images.flags(tile) };
			found = stillCells.emplace(sig, cell).first;
		}
		cells[{ tile, mix }] = found->second;
	}
	// As high as needed (a multiple of 4 rows, 16 at least)
	auto high = 16;
	for (const auto& c : taken) {
		if (!InWaterRegion(c)) {
			high = std::max(high, c.second + 1);
		}
	}
	rows = (high + 3) / 4 * 4;
	auto inside = 0;
	for (const auto& c : taken) {
		inside += InWaterRegion(c) ? 0 : 1;
	}
	used = inside + static_cast<int>(water.size()) * 4;
	image = RgbImage{ ChipColumns * TileSize, rows * TileSize, Key[0], Key[1], Key[2] };
	col = RgbImage{ ChipColumns, rows, Blocked[0], Blocked[1], Blocked[2] };
	for (const auto& [cell, tile] : grid) {
		if (cell.second >= rows) {
			continue;
		}
		const auto scale = TileSize / tile.side;
		for (auto py = 0; py < TileSize; py++) {
			for (auto px = 0; px < TileSize; px++) {
				const auto* v = &tile.rgb[static_cast<std::size_t>(((py / scale) * tile.side + px / scale) * 3)];
				std::copy(v, v + 3, image.at(cell.first * TileSize + px, cell.second * TileSize + py));
			}
		}
	}
	for (const auto& [cell, colour] : colours) {
		if (cell.second < rows) {
			std::copy(colour.begin(), colour.end(), col.at(cell.first, cell.second));
		}
	}
	mask = RgbImage{ ChipColumns * MaskCells, rows * MaskCells, 0, 0, 0 };
	for (const auto& [cell, bits] : masks) {
		if (cell.second >= rows) {
			continue;
		}
		for (auto cy = 0; cy < MaskCells; cy++) {
			for (auto cx = 0; cx < MaskCells; cx++) {
				if ((bits >> (cy * MaskCells + cx)) & 1) {
					auto* pixel = mask.at(cell.first * MaskCells + cx, cell.second * MaskCells + cy);
					pixel[0] = pixel[1] = pixel[2] = 255;
				}
			}
		}
	}
	prop = RgbImage{ ChipColumns, rows, 0, 0, 0 };
	for (const auto& [cell, properties] : bases) {
		if (cell.second < rows) {
			prop.at(cell.first, cell.second)[0] = static_cast<std::uint8_t>(properties.second);
			prop.at(cell.first, cell.second)[1] = static_cast<std::uint8_t>(properties.first + 1);
		}
	}
}

int worldgen::Atlas::mask(const std::string& key) const {
	const auto found = m_masks.find(key);
	return found == m_masks.end() ? 0 : found->second;
}

int worldgen::Atlas::base(const std::string& key) const {
	const auto found = m_bases.find(key);
	return found == m_bases.end() ? -1 : found->second;
}

int worldgen::Atlas::flags(const std::string& key) const {
	const auto found = m_flags.find(key);
	return found == m_flags.end() ? 0 : found->second;
}

std::set<worldgen::TileUse> worldgen::ChunkTiles(const World& world, int cx, int cy) {
	auto used = std::set<TileUse>{};
	const auto& plan = world.terrain().plan();
	for (auto y = cy * Chunk; y < cy * Chunk + Chunk; y++) {
		for (auto x = cx * Chunk; x < cx * Chunk + Chunk; x++) {
			for (const auto layer : { Layer::Base, Layer::Middle, Layer::Top }) {
				const auto tile = world.tile(layer, x, y);
				if (tile != NoTile) {
					used.insert({ tile, BlockMix(plan, x, y) });
				}
			}
		}
	}
	return used;
}

std::set<worldgen::TileUse> worldgen::RoomTiles(const World& world) {
	auto used = std::set<TileUse>{};
	const auto meadow = PureMix(Biome::Meadow);
	for (const auto& room : world.rooms()) {
		for (const auto* grid : { &room.base, &room.middle }) {
			for (const auto tile : *grid) {
				if (tile != NoTile) {
					used.insert({ tile, meadow });
				}
			}
		}
	}
	return used;
}
