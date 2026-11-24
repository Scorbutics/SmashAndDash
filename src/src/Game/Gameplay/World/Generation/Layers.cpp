#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <sstream>
#include <tuple>
#include "Layers.h"
#include "Noise.h"

namespace {
	using namespace worldgen;

	constexpr int RoomMapWidth = 25;  // the smallest map of a room (the screen : 23 x 13)
	constexpr int RoomMapHeight = 15;
	constexpr std::size_t ItemsPerChunk = 2;

	const char* GroundName(Ground ground) {
		switch (ground) {
		case Ground::Water: return "water";
		case Ground::Sand: return "sand";
		case Ground::Path: return "path";
		case Ground::Grass: return "grass";
		default: return "pave";
		}
	}

	// The grounds over one another (PRIORITY) : a higher one reaches into the blocks of a lower one
	int Priority(Ground ground) {
		switch (ground) {
		case Ground::Water: return 0;
		case Ground::Pave: return 1;
		case Ground::Sand: return 2;
		case Ground::Path: return 3;
		default: return 4;
		}
	}

	// The keys of the tiles : the text of the Python keys (repr of their tuples)
	std::string S(const std::string& text) { return "'" + text + "'"; }
	std::string B(bool value) { return value ? "True" : "False"; }
	std::string I(int value) { return std::to_string(value); }
	const std::string None = "None";

	std::string K(std::initializer_list<std::string> items) {
		auto out = std::string{ "(" };
		auto first = true;
		for (const auto& item : items) {
			out += (first ? "" : ", ") + item;
			first = false;
		}
		return out + (items.size() == 1 ? ",)" : ")");
	}

	template <std::size_t N>
	std::string M(const std::array<bool, N>& mask) {
		auto out = std::string{ "(" };
		for (auto k = 0u; k < N; k++) {
			out += (k ? ", " : "") + B(mask[k]);
		}
		return out + ")";
	}

	bool Any(const std::array<bool, 8>& mask) {
		return std::any_of(mask.begin(), mask.end(), [](bool b) { return b; });
	}

	const std::array<std::string, 4> FlowerbedColours{ "roofr", "flower", "roofb", "orange" };

	TileId Flowerbed(std::size_t colour) {
		return AnimatedTile(static_cast<Animated>(static_cast<int>(Animated::FlowerbedRoofr) + static_cast<int>(colour)));
	}

	// The trees : their width, their rows (dy from the block of the canopy bottom, row of the image, kind)
	enum class Part { Top, Canopy, Trunk };
	struct TreePart { int dy, row; Part kind; };
	struct TreeShape { int w; std::vector<TreePart> parts; };

	const TreeShape& TreeShapeOf(const std::string& style) {
		static const auto round = TreeShape{ 3, { { -2, 0, Part::Top }, { -1, 1, Part::Top }, { 0, 2, Part::Canopy }, { 1, 3, Part::Trunk } } };
		static const auto narrow = TreeShape{ 2, { { -1, 0, Part::Top }, { 0, 1, Part::Canopy }, { 1, 3, Part::Trunk } } };
		return style == "round" ? round : narrow;
	}

	// The tree of a biome (tilegen BIOMES)
	std::string BiomeTree(Biome biome) {
		switch (biome) {
		case Biome::Snow: return "snowpine";
		case Biome::Desert: return "cactus";
		default: return "round";
		}
	}

	// The vegetation of the biomes (NATURE)
	struct Nature { double forest, grove, lone, tall, boulder; };
	constexpr std::array<Nature, BiomeCount> NatureTable{ {
		{ 0.52, 0.64, 0.015, 0.60, 0.010 },
		{ 0.47, 0.60, 0.025, 0.58, 0.008 },
		{ 0.50, 0.62, 0.020, 0.64, 0.012 },
		{ 9.00, 9.00, 0.035, 0.66, 0.025 },
	} };
	// The biomes sorted by their names (autumn, desert, meadow, snow)
	constexpr std::array<Biome, BiomeCount> ByName{ Biome::Autumn, Biome::Desert, Biome::Meadow, Biome::Snow };

	// The rectangles of a letter of a room template (each connected group, read as its bounding box)
	std::vector<std::array<int, 4>> Rects(const std::vector<std::string>& rows, char letter) {
		auto seen = Blocks{};
		auto out = std::vector<std::array<int, 4>>{};
		for (auto y = 0; y < static_cast<int>(rows.size()); y++) {
			for (auto x = 0; x < static_cast<int>(rows[static_cast<std::size_t>(y)].size()); x++) {
				if (rows[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] != letter || seen.count({ x, y })) {
					continue;
				}
				auto minX = x, maxX = x, minY = y, maxY = y;
				auto front = std::vector<Block>{ { x, y } };
				seen.insert({ x, y });
				while (!front.empty()) {
					const auto [a, b] = front.back();
					front.pop_back();
					minX = std::min(minX, a);
					maxX = std::max(maxX, a);
					minY = std::min(minY, b);
					maxY = std::max(maxY, b);
					for (const auto& [dx, dy] : { std::pair<int, int>{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } }) {
						const auto nx = a + dx;
						const auto ny = b + dy;
						if (0 <= ny && ny < static_cast<int>(rows.size()) && 0 <= nx && nx < static_cast<int>(rows[static_cast<std::size_t>(ny)].size())
							&& rows[static_cast<std::size_t>(ny)][static_cast<std::size_t>(nx)] == letter && !seen.count({ nx, ny })) {
							seen.insert({ nx, ny });
							front.push_back({ nx, ny });
						}
					}
				}
				out.push_back({ minX, minY, maxX - minX + 1, maxY - minY + 1 });
			}
		}
		return out;
	}

	Shape ParseMask(int w, int h, const std::string& rows) {
		auto shape = Shape{ w, h, {} };
		for (const auto c : rows) {
			if (c != '/') {
				shape.filled.push_back(c == '#');
			}
		}
		shape.filled.resize(static_cast<std::size_t>(w * h), false);
		return shape;
	}
}

const char* worldgen::AnimatedName(Animated animated) {
	static const char* names[AnimatedCount] = { "water", "flower", "tall", "campfire", "lamphead", "butterfly",
		"fountain_0_0", "fountain_0_1", "fountain_1_0", "fountain_1_1",
		"flowerbed_roofr", "flowerbed_flower", "flowerbed_roofb", "flowerbed_orange" };
	return names[static_cast<int>(animated)];
}

bool worldgen::AnimatedWalkable(Animated animated) {
	switch (animated) {
	case Animated::Water:
	case Animated::Campfire:
	case Animated::Fountain00:
	case Animated::Fountain01:
	case Animated::Fountain10:
	case Animated::Fountain11:
		return false;
	default:
		return true;
	}
}

std::string worldgen::TileName(TileId tile) {
	if (tile == NoTile) {
		return "None";
	}
	return IsAnimated(tile) ? std::string{ AnimatedName(AnimatedOf(tile)) } : std::to_string(tile);
}

worldgen::TileId worldgen::Tiles::cell(const std::string& key, bool walkable) {
	auto found = m_ids.find({ key, walkable });
	if (found != m_ids.end()) {
		return found->second;
	}
	const auto tile = static_cast<TileId>(m_keys.size());
	m_ids.emplace(std::make_pair(key, walkable), tile);
	m_keys.emplace_back(key, walkable);
	return tile;
}

worldgen::Shapes worldgen::Shapes::Parse(std::istream& in) {
	auto shapes = Shapes{};
	auto line = std::string{};
	while (std::getline(in, line)) {
		auto words = std::istringstream{ line };
		auto kind = std::string{};
		if (!(words >> kind)) {
			continue;
		}
		if (kind == "stamp") {
			auto name = std::string{}, rows = std::string{};
			auto w = 0, h = 0;
			words >> name >> w >> h >> rows;
			shapes.stamps[name] = ParseMask(w, h, rows);
		} else if (kind == "furniture") {
			auto letter = std::string{}, extra = std::string{}, rows = std::string{};
			auto w = 0, h = 0;
			words >> letter >> w >> h >> extra >> rows;
			shapes.furniture[letter + " " + std::to_string(w) + " " + std::to_string(h) + " " + extra] = ParseMask(w, h, rows);
		}
	}
	return shapes;
}

worldgen::RoomTemplate worldgen::RoomTemplate::Parse(const std::string& name, std::istream& in) {
	auto room = RoomTemplate{ name, {} };
	auto line = std::string{};
	while (std::getline(in, line)) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		if (line.find_first_not_of(" \t") != std::string::npos) {
			room.rows.push_back(line);
		}
	}
	return room;
}

std::vector<worldgen::RoomTemplate> worldgen::LoadRoomTemplates(const std::string& directory) {
	auto names = std::vector<std::string>{};
	for (const auto& entry : std::filesystem::directory_iterator{ directory }) {
		const auto file = entry.path().filename().string();
		if (entry.path().extension() == ".txt") {
			names.push_back(file);
		}
	}
	std::sort(names.begin(), names.end());
	auto out = std::vector<RoomTemplate>{};
	for (const auto& file : names) {
		auto in = std::ifstream{ directory + "/" + file };
		out.push_back(RoomTemplate::Parse(file.substr(0, file.size() - 4), in));
	}
	return out;
}

worldgen::World::World(const Terrain& terrain, const Shapes& shapes, const std::vector<RoomTemplate>& rooms) :
	m_terrain(terrain), m_shapes(shapes), m_seed(terrain.plan().climate().seed()) {
	const auto size = static_cast<std::size_t>(width() * height());
	for (auto& layer : m_layers) {
		layer.assign(size, NoTile);
	}
	m_walk.assign(size, 1);
	build();
	placeTown();
	placePois();
	for (const auto& [x, y] : m_terrain.stairs()) { // nothing in the way of the stairs (trees, boulders)
		for (const auto dy : { -2, -1, 2, 3 }) {
			take(x, y + dy);
		}
	}
	placeNature();
	placeSigns();
	placeItems();
	placeRooms(rooms);
}

void worldgen::World::set(Layer layer, int x, int y, TileId tile) {
	if (inMap(x, y)) {
		at(layer, x, y) = tile;
	}
}

bool worldgen::World::plain(int x, int y) const {
	if (mat(x, y) != Ground::Grass) {
		return false;
	}
	const auto lv = level(x, y);
	return std::all_of(D8.begin(), D8.end(), [&](const std::array<int, 2>& d) {
		return mat(x + d[0], y + d[1]) == Ground::Grass && level(x + d[0], y + d[1]) == lv;
	});
}

void worldgen::World::build() {
	const auto& stairs = m_terrain.stairs();
	for (auto y = 0; y < height(); y++) {
		for (auto x = 0; x < width(); x++) {
			const auto m = mat(x, y);
			const auto lv = level(x, y);
			const auto low = m_terrain.lowMask(x, y);
			if (Any(low)) {
				const auto onStairs = stairs.count({ x, y }) > 0;
				const auto foot = low[2] && m_terrain.isFoot(x, y + 1);
				const auto walk = onStairs || !(low[0] || low[1] || low[2] || low[3]);
				set(Layer::Base, x, y, m_tiles.cell(K({ S("cliff"), M(low), B(onStairs), B(foot) }), walk));
				m_walk[index(x, y)] = walk;
				continue;
			}
			if (m == Ground::Water) {
				set(Layer::Base, x, y, AnimatedTile(Animated::Water));
				m_walk[index(x, y)] = 0;
				const auto mk = Mask8([this](int a, int b) { return mat(a, b) != Ground::Water; }, x, y);
				if (Any(mk)) {
					auto over = Ground::Water;
					auto found = false;
					for (auto k = 0u; k < 8; k++) {
						const auto g = mat(x + D8[k][0], y + D8[k][1]);
						if (mk[k] && (!found || Priority(g) > Priority(over))) {
							over = g;
							found = true;
						}
					}
					over = over == Ground::Path ? Ground::Sand : over;
					set(Layer::Middle, x, y, m_tiles.cell(K({ S("bank"), S(GroundName(over)), M(mk) }), true));
				}
				continue;
			}
			if (m_terrain.isFoot(x, y)) {
				const auto onStairs = stairs.count({ x, y - 1 }) > 0;
				const auto west = m_terrain.isFoot(x - 1, y);
				const auto east = m_terrain.isFoot(x + 1, y);
				set(Layer::Base, x, y, m_tiles.cell(K({ S("foot"), S(GroundName(m)), B(west), B(east), B(onStairs) }), onStairs));
				m_walk[index(x, y)] = onStairs;
				m_objects[{ x, y }] = 0.5;
				continue;
			}
			const auto shade = level(x, y - 1) > lv;
			auto over = Ground::Water;
			auto higher = false;
			for (const auto& d : D8) {
				const auto g = mat(x + d[0], y + d[1]);
				if (g != Ground::Water && Priority(g) > Priority(m) && level(x + d[0], y + d[1]) == lv && (!higher || Priority(g) > Priority(over))) {
					over = g;
					higher = true;
				}
			}
			if (higher) {
				const auto mk = Mask8([this, over, lv](int a, int b) { return mat(a, b) == over && level(a, b) == lv; }, x, y);
				set(Layer::Base, x, y, m_tiles.cell(K({ S("ground"), S(GroundName(m)), S(GroundName(over)), M(mk), B(shade) }), true));
				continue;
			}
			if (m == Ground::Grass && !shade) {
				const auto r = H01(x, y, m_seed + 97);
				if (r < 0.012 && !m_terrain.inTown(x, y, 4)) {
					set(Layer::Base, x, y, AnimatedTile(Animated::Flower));
					continue;
				}
				const auto v = r < 0.5 ? 0 : r < 0.75 ? 1 : r < 0.92 ? 3 : 2;
				set(Layer::Base, x, y, m_tiles.cell(K({ S("grass"), I(v) }), true));
			} else {
				set(Layer::Base, x, y, m_tiles.cell(K({ S("ground"), S(GroundName(m)), None, None, B(shade), I(0) }), true));
			}
		}
	}
}

void worldgen::World::stamp(const std::string& name, int x0, int y0, std::optional<std::pair<int, int>> door) {
	const auto& shape = m_shapes.stamps.at(name);
	for (auto r = 0; r < shape.h; r++) {
		for (auto c = 0; c < shape.w; c++) {
			if (!shape.at(c, r)) {
				continue;
			}
			const auto x = x0 + c;
			const auto y = y0 + r;
			const auto key = K({ S("stamp"), S(name), I(c), I(r) });
			take(x, y);
			m_objects[{ x, y }] = 1.3;
			if (r == 0) {
				set(Layer::Top, x, y, m_tiles.cell(key, true));
			} else {
				const auto walk = door.has_value() && *door == std::make_pair(c, r);
				set(Layer::Base, x, y, m_tiles.cell(key, walk));
				m_walk[index(x, y)] = walk;
				set(Layer::Middle, x, y, NoTile);
			}
		}
	}
}

void worldgen::World::placeTown() {
	for (const auto& v : m_terrain.plan().villages()) {
		const auto biome = std::string{ BiomeName(v.biome) };
		for (const auto& b : v.buildings) {
			const auto name = b.kind == "center" ? "center_" + biome : "house_" + b.roof + "_" + biome;
			stamp(name, b.x0, b.y0, std::make_pair(b.door, b.doorRow));
		}
		const auto colour = static_cast<std::size_t>(H01(v.center.first, v.center.second, 9) * FlowerbedColours.size());
		for (const auto& [x, y] : v.flowers) { // flower beds along some houses (they sway)
			if (mat(x, y) == Ground::Grass && !taken(x, y)) {
				set(Layer::Middle, x, y, Flowerbed(colour));
				take(x, y);
			}
		}
		m_taken.insert(v.cells.begin(), v.cells.end());
		decorate(v);
	}
	m_taken.insert(m_terrain.routes().begin(), m_terrain.routes().end());
}

bool worldgen::World::decoOk(int x, int y) const {
	if (!inMap(x, y) || taken(x, y) || !walk(x, y)) {
		return false;
	}
	const auto cell = m_terrain.cell(x, y);
	return (cell == Cell{ Ground::Grass, 0 } || cell == Cell{ Ground::Sand, 0 }) && m_terrain.flat(x, y)
		&& tile(Layer::Middle, x, y) == NoTile && 1 < x % Chunk && x % Chunk < Chunk - 2 && 1 < y % Chunk && y % Chunk < Chunk - 2;
}

void worldgen::World::putObject(Layer layer, int x, int y, TileId tile, double height) {
	set(layer, x, y, tile);
	m_walk[index(x, y)] = 0;
	take(x, y);
	m_objects[{ x, y }] = height;
}

void worldgen::World::decorate(const Village& v) {
	auto rng = Rng{ m_seed + 77 + v.center.first };
	const auto [cx, cy] = v.center;
	const auto onRoad = [&v](int x, int y) { return v.road.count({ x, y }) > 0; };
	if (onRoad(cx - 1, cy - 1) && onRoad(cx - 1, cy) && onRoad(cx, cy - 1) && onRoad(cx, cy)) {
		const auto fountain = rng.random() < 0.5; // a fountain on the plaza, else a painted emblem
		for (auto c = 0; c < 2; c++) {
			for (auto r = 0; r < 2; r++) {
				const auto x = cx - 1 + c;
				const auto y = cy - 1 + r;
				if (fountain) {
					set(Layer::Middle, x, y, AnimatedTile(static_cast<Animated>(static_cast<int>(Animated::Fountain00) + c * 2 + r)));
					m_walk[index(x, y)] = 0;
					take(x, y);
					m_objects[{ x, y }] = 0.4;
				} else {
					set(Layer::Middle, x, y, m_tiles.cell(K({ S("emblem"), I(c), I(r) }), true));
				}
			}
		}
	}
	// Street lamps along the streets
	auto lamps = std::vector<Block>{};
	auto road = std::vector<std::tuple<double, int, int>>{};
	for (const auto& [x, y] : v.road) {
		road.emplace_back(H01(x, y, m_seed + 61), x, y);
	}
	std::sort(road.begin(), road.end());
	for (const auto& [h, x, y] : road) {
		const auto close = std::any_of(lamps.begin(), lamps.end(), [x = x, y = y](const Block& l) { return std::abs(x - l.first) + std::abs(y - l.second) < 7; });
		if (close || lamps.size() > 8) {
			continue;
		}
		for (const auto& [dx, dy] : { std::pair<int, int>{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } }) {
			const auto lx = x + dx;
			const auto ly = y + dy;
			if (!onRoad(lx, ly) && decoOk(lx, ly) && tile(Layer::Top, lx, ly - 1) == NoTile && !v.footprint.count({ lx, ly - 1 })) {
				const auto post = m_tiles.cell(K({ S("lamp"), I(1) }), false);
				set(Layer::Top, lx, ly - 1, AnimatedTile(Animated::LampHead));
				set(Layer::Middle, lx, ly, post);
				m_walk[index(lx, ly)] = 0;
				take(lx, ly);
				m_objects[{ lx, ly }] = 0.6;
				lamps.push_back({ lx, ly });
				break;
			}
		}
	}
	// Benches around the plaza, a bin by one of them
	auto benches = 0;
	for (auto i = 0; i < 60; i++) {
		if (benches >= 2) {
			break;
		}
		const auto bx = cx + rng.randint(-5, 5);
		const auto by = cy + rng.randint(-4, 4);
		if (onRoad(bx, by) || !decoOk(bx, by) || !(onRoad(bx + 1, by) || onRoad(bx - 1, by) || onRoad(bx, by + 1) || onRoad(bx, by - 1))) {
			continue;
		}
		putObject(Layer::Middle, bx, by, m_tiles.cell(K({ S("bench") }), false), 0.3);
		benches++;
		if (benches == 1 && decoOk(bx + 1, by) && !onRoad(bx + 1, by)) {
			putObject(Layer::Middle, bx + 1, by, m_tiles.cell(K({ S("bin") }), false), 0.4);
		}
	}
	const auto pot = FlowerbedColours[static_cast<std::size_t>(H01(cx, cy, 31) * FlowerbedColours.size())];
	for (const auto& b : v.buildings) {
		if (b.kind == "house") { // the mailbox, beside the path to the door ; a pot of flowers
			const auto mx = b.x0 + b.door + 1;
			const auto my = b.y0 + b.h;
			if (decoOk(mx, my)) {
				set(Layer::Middle, mx, my, m_tiles.cell(K({ S("mailbox") }), false));
				m_walk[index(mx, my)] = 0;
				take(mx, my);
			}
			if (decoOk(b.x0 + b.door - 1, my)) {
				putObject(Layer::Middle, b.x0 + b.door - 1, my, m_tiles.cell(K({ S("flowerpot"), S(pot) }), false), 0.4);
			}
		}
		if ((v.biome == Biome::Desert || v.biome == Biome::Autumn) && rng.random() < 0.6 && decoOk(b.x0 + b.w, b.y0 + b.h - 1)) {
			putObject(Layer::Middle, b.x0 + b.w, b.y0 + b.h - 1, m_tiles.cell(K({ S("barrel") }), false), 0.5);
		}
		if (rng.random() < 0.6) { // bushes along a side
			const auto side = rng.random() < 0.5 ? b.x0 - 1 : b.x0 + b.w;
			for (auto y = b.y0 + 1; y < b.y0 + b.h; y++) {
				if (decoOk(side, y)) {
					set(Layer::Middle, side, y, m_tiles.cell(K({ S("bush") }), false));
					m_walk[index(side, y)] = 0;
					take(side, y);
					m_objects[{ side, y }] = 0.5;
				}
			}
		}
	}
	// A vegetable garden : 4 x 2 rows of crops, fenced on its north and sides (open to the south)
	auto garden = std::optional<Block>{};
	for (auto i = 0; i < 80; i++) {
		const auto gx = cx + rng.randint(-16, 12);
		const auto gy = cy + rng.randint(-14, 12);
		auto ok = true;
		for (auto j = 0; j < 3 && ok; j++) {
			for (auto k = 0; k < 6 && ok; k++) {
				ok = decoOk(gx + k, gy + j) && m_terrain.cell(gx + k, gy + j) == Cell{ Ground::Grass, 0 };
			}
		}
		for (auto x = gx + 1; x < gx + 5 && ok; x++) {
			ok = decoOk(x, gy + 3);
		}
		if (!ok) {
			continue;
		}
		for (auto k = 0; k < 6; k++) {
			const auto kind = k == 0 ? S("tl") : k == 5 ? S("tr") : None;
			set(Layer::Middle, gx + k, gy, m_tiles.cell(K({ S("fence"), kind }), false));
			m_walk[index(gx + k, gy)] = 0;
		}
		for (const auto j : { 1, 2 }) {
			for (const auto k : { 0, 5 }) {
				set(Layer::Middle, gx + k, gy + j, m_tiles.cell(K({ S("fence"), S("v") }), false));
				m_walk[index(gx + k, gy + j)] = 0;
			}
			for (auto k = 1; k < 5; k++) {
				set(Layer::Middle, gx + k, gy + j, m_tiles.cell(K({ S("crops") }), true));
			}
		}
		for (auto j = 0; j < 3; j++) {
			for (auto k = 0; k < 6; k++) {
				take(gx + k, gy + j);
			}
		}
		garden = Block{ gx, gy };
		break;
	}
	// A well
	for (auto i = 0; i < 60; i++) {
		const auto wx = cx + rng.randint(-12, 12);
		const auto wy = cy + rng.randint(-10, 10);
		auto ok = true;
		for (auto dx = -1; dx <= 1 && ok; dx++) {
			for (auto dy = -1; dy <= 1 && ok; dy++) {
				ok = decoOk(wx + dx, wy + dy);
			}
		}
		if (ok) {
			putObject(Layer::Middle, wx, wy, m_tiles.cell(K({ S("well") }), false), 0.6);
			break;
		}
	}
	// Butterflies over the grass, near the flowers and the garden
	auto near = std::vector<Block>(v.flowers.begin(), v.flowers.end());
	if (garden) {
		near.push_back({ garden->first + 2, garden->second + 3 });
	}
	auto flown = 0;
	for (auto i = 0; i < 60; i++) {
		if (flown >= 3 || near.empty()) {
			break;
		}
		const auto [fx, fy] = near[rng.choice(near.size())];
		const auto bx = fx + rng.randint(-3, 3);
		const auto by = fy + rng.randint(-2, 3);
		if (decoOk(bx, by) && m_terrain.cell(bx, by) == Cell{ Ground::Grass, 0 } && plain(bx, by) && tile(Layer::Top, bx, by) == NoTile) {
			set(Layer::Base, bx, by, AnimatedTile(Animated::Butterfly));
			take(bx, by);
			flown++;
		}
	}
	// Water lilies on the pond
	auto lilies = 0;
	for (const auto& [x, y] : v.pond) {
		if (lilies < 2 && tile(Layer::Middle, x, y) == NoTile) {
			set(Layer::Middle, x, y, m_tiles.cell(K({ S("lily") }), false));
			lilies++;
		}
	}
	// Broadleaf trees between the houses
	auto planted = 0;
	for (auto i = 0; i < 80; i++) {
		if (planted >= 5) {
			break;
		}
		const auto tx = cx + rng.randint(-18, 15);
		const auto ty = cy + rng.randint(-15, 15);
		const auto distance = m_terrain.plan().nearVillage().find({ tx, ty });
		if ((distance != m_terrain.plan().nearVillage().end() && distance->second < 1) || !m_terrain.inTown(tx, ty, 4)) {
			continue;
		}
		const auto style = BiomeTree(v.biome);
		auto ok = true;
		for (auto c = 0; c < TreeShapeOf(style).w && ok; c++) {
			for (auto dy = -2; dy <= 1 && ok; dy++) {
				ok = decoOk(tx + c, ty + dy);
			}
		}
		if (ok && treeAt(tx, ty, style)) {
			planted++;
		}
	}
}

std::string worldgen::World::treeStyle(int x, int y) const {
	auto sum = 0.0;
	auto style = std::string{};
	for (const auto b : ByName) {
		const auto w = weight(x, y, b);
		if (w > 0.0) {
			sum += w;
			style = BiomeTree(b);
		}
	}
	auto r = H01(x, y, m_seed + 403) * sum;
	for (const auto b : ByName) {
		const auto w = weight(x, y, b);
		if (w > 0.0) {
			r -= w;
			if (r <= 0) {
				style = BiomeTree(b);
				break;
			}
		}
	}
	if (style == "round" && H01(x, y, m_seed + 404) < 0.15) {
		style = "pine"; // a few pines among the broadleaf trees
	}
	return style;
}

bool worldgen::World::freeCell(int x, int y) const {
	return !taken(x, y) && plain(x, y) && tile(Layer::Base, x, y) != AnimatedTile(Animated::Flower);
}

bool worldgen::World::treeAt(int x, int y, const std::string& style) {
	const auto& shape = TreeShapeOf(style);
	for (auto c = 0; c < shape.w; c++) {
		for (const auto& part : shape.parts) {
			const auto cx = x + c;
			const auto cy = y + part.dy;
			if (!inMap(cx, cy)) {
				return false;
			}
			if (part.kind == Part::Top) {
				if (tile(Layer::Top, cx, cy) != NoTile || !plain(cx, cy)) {
					return false;
				}
			} else if (!freeCell(cx, cy)) {
				return false;
			}
		}
	}
	for (auto c = 0; c < shape.w; c++) {
		for (const auto& part : shape.parts) {
			const auto cx = x + c;
			const auto cy = y + part.dy;
			const auto key = K({ S("tree"), S(style), I(part.row), I(c) });
			if (part.kind == Part::Top) {
				set(Layer::Top, cx, cy, m_tiles.cell(key, true));
				auto& object = m_objects[{ cx, cy }];
				object = std::max(object, 1.2);
				continue;
			}
			const auto cell = m_tiles.cell(key, false);
			set(Layer::Base, cx, cy, cell);
			if (part.kind == Part::Canopy) {
				set(Layer::Top, cx, cy, cell);
			}
			take(cx, cy);
			m_objects[{ cx, cy }] = 1.6;
			m_walk[index(cx, cy)] = 0;
		}
	}
	return true;
}

void worldgen::World::placeNature() {
	const auto nature = [this](int x, int y) {
		auto total = 0.0;
		auto out = Nature{ 0.0, 0.0, 0.0, 0.0, 0.0 };
		for (auto b = 0; b < BiomeCount; b++) {
			total += weight(x, y, static_cast<Biome>(b));
		}
		for (auto b = 0; b < BiomeCount; b++) {
			const auto w = weight(x, y, static_cast<Biome>(b));
			const auto& n = NatureTable[static_cast<std::size_t>(b)];
			out.forest += n.forest * w;
			out.grove += n.grove * w;
			out.lone += n.lone * w;
			out.tall += n.tall * w;
			out.boulder += n.boulder * w;
		}
		return Nature{ out.forest / total, out.grove / total, out.lone / total, out.tall / total, out.boulder / total };
	};
	// Forests : packed, every 2 blocks, the rows shifted by one (a wall of trees), their edges a bit ragged ; groves :
	// small packed clumps ; a few lone trees. The broadleaf trees (3 wide), then the others (2 wide)
	for (const auto& [wide, step] : { std::pair<bool, int>{ true, 3 }, { false, 2 } }) {
		for (auto y = 2; y < height() - 2; y += step) {
			for (auto x0 = 0; x0 < width() - step; x0 += step) {
				const auto x = x0 + (y / step) % step;
				if (m_terrain.inTown(x, y, 4) || (treeStyle(x, y) == "round") != wide) {
					continue;
				}
				const auto forest = Fbm(x / 13.0, y / 13.0, m_seed + 1);
				const auto grove = Fbm(x / 4.0, y / 4.0, m_seed + 3);
				const auto r = H01(x, y, m_seed + 12);
				const auto p = nature(x, y);
				const auto dense = (forest > p.forest && r < 0.95) || (forest > p.forest - 0.05 && r < 0.4)
					|| (grove > p.grove && r < 0.9) || r < p.lone;
				if (dense) {
					treeAt(x, y, treeStyle(x, y));
				}
			}
		}
	}
	for (auto y = 0; y < height(); y++) {
		for (auto x = 0; x < width(); x++) {
			if (!freeCell(x, y) || m_terrain.inTown(x, y, 2)) {
				continue;
			}
			const auto p = nature(x, y);
			if (Fbm(x / 6.0, y / 6.0, m_seed + 2) > p.tall && Fbm(x / 13.0, y / 13.0, m_seed + 1) < 0.5) {
				set(Layer::Middle, x, y, AnimatedTile(Animated::Tall));
			} else if (H01(x, y, m_seed + 21) < p.boulder) {
				set(Layer::Middle, x, y, m_tiles.cell(K({ S("boulder") }), false));
				m_walk[index(x, y)] = 0;
				m_objects[{ x, y }] = 0.4;
			}
		}
	}
}

void worldgen::World::addEvent(int x, int y, const std::string& line) {
	const auto cx = x / Chunk;
	const auto cy = y / Chunk;
	m_events[{ cx, cy }].push_back(std::to_string(x - cx * Chunk) + " " + std::to_string(y - cy * Chunk) + " " + line);
}

void worldgen::World::itemEvent(int x, int y, int item) {
	const auto cx = x / Chunk;
	const auto cy = y / Chunk;
	const auto lx = x - cx * Chunk;
	const auto ly = y - cy * Chunk;
	addEvent(cx * Chunk, cy * Chunk, "0 Levels/item.miniska, place, " + std::to_string(item) + ", " + std::to_string(lx) + ", " + std::to_string(ly)
		+ ", item_w_" + std::to_string(cx) + "_" + std::to_string(cy) + "_" + std::to_string(lx) + "_" + std::to_string(ly));
	take(x, y);
	m_items.push_back({ x, y, item });
}

void worldgen::World::placePois() {
	for (const auto& p : m_terrain.pois()) {
		const auto ax = p.x;
		const auto ay = p.y;
		for (auto j = 0; j < p.h; j++) {
			for (auto i = 0; i < p.w; i++) {
				take(ax + i, ay + j);
			}
		}
		m_taken.insert(p.spur.begin(), p.spur.end());
		if (p.kind == "camp") {
			static const std::array<std::string, 4> canvases{ "roofo", "roofg", "roofb", "roofr" };
			const auto canvas = canvases[static_cast<std::size_t>(H01(ax, ay, 3) * 4)];
			stamp("tent_" + canvas, ax + 1, ay, std::nullopt);
			putObject(Layer::Middle, ax + 3, ay + 3, AnimatedTile(Animated::Campfire), 0.3);
			putObject(Layer::Middle, ax + 2, ay + 3, m_tiles.cell(K({ S("log seat") }), false), 0.3);
			putObject(Layer::Middle, ax + 4, ay + 3, m_tiles.cell(K({ S("log seat") }), false), 0.3);
			itemEvent(ax + 4, ay + 1, H01(ax, ay, 9) < 0.6 ? 0 : 2);
		} else if (p.kind == "ruins") {
			for (const auto& [px, py] : { std::pair<int, int>{ 0, 0 }, { 3, 0 }, { 6, 0 }, { 0, 4 }, { 6, 4 }, { 0, 2 }, { 6, 2 } }) {
				const auto broken = H01(ax + px, ay + py, 11) < 0.45;
				stamp(std::string{ "pillar_" } + (broken ? "True" : "False"), ax + px, ay + py, std::nullopt);
			}
			for (auto i = 0; i < 2; i++) {
				const auto bx = ax + 1 + static_cast<int>(H01(ax, ay, 20 + i) * 5);
				const auto by = ay + 6;
				if (bx != ax + 3) {
					putObject(Layer::Middle, bx, by, m_tiles.cell(K({ S("boulder") }), false), 0.4);
				}
			}
			itemEvent(ax + 3, ay + 3, H01(ax, ay, 13) < 0.6 ? 1 : 2);
		} else {
			const auto& roofs = StyleOf(p.biome).roofs;
			const auto roof = roofs[static_cast<std::size_t>(H01(ax, ay, 5) * static_cast<double>(roofs.size()))];
			stamp("house_" + roof + "_" + BiomeName(p.biome), ax + 1, ay + p.h - 5, std::make_pair(2, 3));
			m_loneBuildings.push_back(Building{ "rest", roof, ax + 1, ay + p.h - 5, 4, 4, 2, 3 });
		}
	}
}

void worldgen::World::placeSigns() {
	const auto& villages = m_terrain.plan().villages();
	const auto& near = m_terrain.plan().nearVillage();
	for (auto i = 0; i < static_cast<int>(villages.size()); i++) {
		const auto& v = villages[static_cast<std::size_t>(i)];
		for (const auto& route : m_terrain.routePaths()) {
			if (route.from != i && route.to != i) {
				continue;
			}
			auto path = route.path;
			if (route.from != i) {
				std::reverse(path.begin(), path.end());
			}
			auto out = std::vector<Block>{};
			for (const auto& c : path) {
				const auto d = near.find(c);
				if (!v.cells.count(c) && (d == near.end() || d->second >= 2)) {
					out.push_back(c);
				}
			}
			if (out.empty()) {
				continue;
			}
			const auto [x, y] = out[0];
			const auto [nx, ny] = out[std::min<std::size_t>(1, out.size() - 1)];
			const auto ax = ny - y;
			const auto ay = nx - x;
			for (const auto& [sx, sy] : { Block{ x + ax, y + ay }, Block{ x - ax, y - ay }, Block{ x + ax * 2, y + ay * 2 }, Block{ x - ax * 2, y - ay * 2 } }) {
				if (inMap(sx, sy) && !taken(sx, sy) && walk(sx, sy) && tile(Layer::Middle, sx, sy) == NoTile && m_terrain.flat(sx, sy)) {
					set(Layer::Middle, sx, sy, m_tiles.cell(K({ S("sign") }), false));
					m_walk[index(sx, sy)] = 0;
					take(sx, sy);
					m_objects[{ sx, sy }] = 0.4;
					addEvent(sx, sy, "1 Levels/sign.miniska, " + v.name + "\xa4" + StyleOf(v.biome).line);
					break;
				}
			}
		}
	}
}

std::vector<char> worldgen::World::reachable() const {
	auto seen = std::vector<char>(static_cast<std::size_t>(width() * height()), 0);
	const auto start = Block{ m_terrain.plan().town().first, m_terrain.plan().town().second + 1 };
	seen[index(start.first, start.second)] = 1;
	auto front = std::vector<Block>{ start };
	while (!front.empty()) {
		const auto [x, y] = front.back();
		front.pop_back();
		for (auto k = 0u; k < 4; k++) {
			const auto nx = x + D8[k][0];
			const auto ny = y + D8[k][1];
			if (inMap(nx, ny) && !seen[index(nx, ny)] && walk(nx, ny)) {
				seen[index(nx, ny)] = 1;
				front.push_back({ nx, ny });
			}
		}
	}
	return seen;
}

void worldgen::World::placeItems() {
	const auto reach = reachable();
	const auto& nearRoute = m_terrain.nearRoute();
	for (auto cy = 0; cy < height() / Chunk; cy++) {
		for (auto cx = 0; cx < width() / Chunk; cx++) {
			auto cands = std::vector<std::tuple<double, int, int>>{};
			for (auto y = cy * Chunk + 3; y < cy * Chunk + Chunk - 3; y++) {
				for (auto x = cx * Chunk + 3; x < cx * Chunk + Chunk - 3; x++) {
					const auto road = nearRoute.find({ x, y });
					if (!reach[index(x, y)] || taken(x, y) || m_terrain.inTown(x, y, 4) || (road != nearRoute.end() && road->second <= 3)
						|| tile(Layer::Middle, x, y) != NoTile) {
						continue;
					}
					auto open = true;
					for (auto dx = -1; dx <= 1 && open; dx++) {
						for (auto dy = -1; dy <= 1 && open; dy++) {
							open = walk(x + dx, y + dy);
						}
					}
					if (!open) {
						continue;
					}
					auto trees = 0;
					for (auto dx = -3; dx <= 3; dx++) {
						for (auto dy = -3; dy <= 3; dy++) {
							const auto object = m_objects.find({ x + dx, y + dy });
							trees += object != m_objects.end() && object->second >= 1.6 ? 1 : 0;
						}
					}
					cands.emplace_back(1.5 * level(x, y) + 0.15 * trees + H01(x, y, m_seed + 77), x, y);
				}
			}
			std::sort(cands.begin(), cands.end(), std::greater<>{});
			auto chosen = std::vector<Block>{};
			for (const auto& [score, x, y] : cands) {
				if (chosen.size() >= ItemsPerChunk) {
					break;
				}
				const auto close = std::any_of(chosen.begin(), chosen.end(), [x = x, y = y](const Block& c) {
					return (x - c.first) * (x - c.first) + (y - c.second) * (y - c.second) < 144;
				});
				if (score < 1.2 || close) {
					continue;
				}
				chosen.push_back({ x, y });
			}
			for (const auto& [x, y] : chosen) {
				auto r = H01(x, y, m_seed + 78);
				auto item = 2;
				for (const auto& [id, chance] : { std::pair<int, double>{ 0, 0.5 }, { 1, 0.25 }, { 2, 0.25 } }) {
					if (r < chance) {
						item = id;
						break;
					}
					r -= chance;
				}
				itemEvent(x, y, item);
			}
		}
	}
}

void worldgen::World::placeRooms(const std::vector<RoomTemplate>& templates) {
	auto buildings = std::vector<Building>{};
	for (const auto& v : m_terrain.plan().villages()) {
		buildings.insert(buildings.end(), v.buildings.begin(), v.buildings.end());
	}
	buildings.insert(buildings.end(), m_loneBuildings.begin(), m_loneBuildings.end());
	for (auto i = 0; i < static_cast<int>(buildings.size()); i++) {
		const auto& building = buildings[static_cast<std::size_t>(i)];
		const auto& kind = building.kind;
		auto choices = std::vector<const RoomTemplate*>{};
		for (const auto& t : templates) {
			if (t.name.rfind(kind + "_", 0) == 0) {
				choices.push_back(&t);
			}
		}
		const auto& chosen = *choices[static_cast<std::size_t>(H01(i, m_seed, 31) * static_cast<double>(choices.size()))];
		const auto& rows = chosen.rows;
		const auto rw = static_cast<int>(rows[0].size());
		const auto rh = static_cast<int>(rows.size());
		const auto letter = [&rows](int x, int y) { return rows[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)]; };

		// The room (rooms.build_room) : the floor and the walls, then the furniture
		auto base = std::vector<TileId>(static_cast<std::size_t>(rw * rh), NoTile);
		auto mid = std::vector<TileId>(static_cast<std::size_t>(rw * rh), NoTile);
		auto walk = std::vector<bool>(static_cast<std::size_t>(rw * rh), false);
		const auto at = [rw](int x, int y) { return static_cast<std::size_t>(y * rw + x); };
		for (auto y = 0; y < rh; y++) {
			for (auto x = 0; x < rw; x++) {
				const auto c = letter(x, y);
				walk[at(x, y)] = c == '.' || c == 'E' || c == 'R' || c == 'e';
			}
		}
		const auto paper = kind == "center" ? "white" : "wall";
		const auto floor = m_tiles.cell(K({ S("room floor"), S(kind) }), true);
		for (auto y = 0; y < rh; y++) {
			for (auto x = 0; x < rw; x++) {
				const auto c = letter(x, y);
				if (c == '#') {
					base[at(x, y)] = m_tiles.cell(K({ S("room wall top") }), false);
					walk[at(x, y)] = false;
				} else if (c == '=' || c == 'W' || c == 'F') {
					const auto item = c == '=' ? None : S(std::string(1, c));
					base[at(x, y)] = m_tiles.cell(K({ S("room wall"), item, S(paper) }), false);
					walk[at(x, y)] = false;
				} else if (c == 'R') {
					auto mk = std::array<bool, 4>{};
					const std::array<std::array<int, 2>, 4> sides{ { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } } };
					for (auto k = 0u; k < 4; k++) {
						const auto nx = x + sides[k][0];
						const auto ny = y + sides[k][1];
						mk[k] = 0 <= ny && ny < rh && 0 <= nx && nx < rw && letter(nx, ny) == 'R';
					}
					base[at(x, y)] = m_tiles.cell(K({ S("room rug"), M(mk) }), true);
				} else if (c == 'E') {
					base[at(x, y)] = m_tiles.cell(K({ S("room mat") }), true);
				} else {
					base[at(x, y)] = floor;
				}
			}
		}
		static const std::array<std::string, 3> blankets{ "roofb", "roofr", "roofg" };
		const auto blanket = blankets[static_cast<std::size_t>(H01(m_seed + i, 1, 7) * 3)];
		auto counter = std::vector<Block>{};
		for (const auto c : std::string{ "BTcPSKC" }) {
			for (const auto& [x0, y0, cw, ch] : Rects(rows, c)) {
				const auto onWall = c == 'S' || c == 'K';
				const auto top = onWall ? y0 - 1 : y0;
				const auto th = ch + (onWall ? 1 : 0);
				const auto extra = c == 'B' ? S(blanket) : None;
				const auto& shape = m_shapes.furniture.at(std::string(1, c) + " " + std::to_string(cw) + " " + std::to_string(th) + " " + (c == 'B' ? blanket : "-"));
				for (auto r = 0; r < th; r++) {
					for (auto k = 0; k < cw; k++) {
						if (!shape.at(k, r)) {
							continue;
						}
						mid[at(x0 + k, top + r)] = m_tiles.cell(K({ S("room furniture"), S(std::string(1, c)), I(cw), I(th), extra, I(k), I(r) }), false);
						walk[at(x0 + k, top + r)] = false;
					}
				}
				if (c == 'C') {
					for (auto x = x0; x < x0 + cw; x++) {
						counter.push_back({ x, y0 + ch - 1 });
					}
				}
			}
		}
		for (const auto& [x0, y0, cw, ch] : Rects(rows, 'H')) {
			mid[at(x0, y0)] = m_tiles.cell(K({ S("room furniture"), S("H") }), false);
			walk[at(x0, y0)] = false;
		}
		auto exits = std::vector<Block>{};
		auto beds = std::vector<Block>{};
		for (auto y = 0; y < rh; y++) {
			for (auto x = 0; x < rw; x++) {
				if (letter(x, y) == 'E') {
					exits.push_back({ x, y });
				} else if (letter(x, y) == 'B') {
					beds.push_back({ x, y });
				}
			}
		}

		// Centred in a map at least as large as the screen (the camera keeps a smaller map in a corner) ; the empty
		// blocks around are void : they block the player
		auto room = Room{};
		room.width = std::max(rw, RoomMapWidth);
		room.height = std::max(rh, RoomMapHeight);
		const auto ox = (room.width - rw) / 2;
		const auto oy = (room.height - rh) / 2;
		const auto size = static_cast<std::size_t>(room.width * room.height);
		room.base.assign(size, NoTile);
		room.middle.assign(size, NoTile);
		room.walk.assign(size, false);
		for (auto y = 0; y < room.height; y++) {
			auto line = std::string{};
			for (auto x = 0; x < room.width; x++) {
				const auto inside = 0 <= x - ox && x - ox < rw && 0 <= y - oy && y - oy < rh;
				const auto out = static_cast<std::size_t>(y * room.width + x);
				if (inside) {
					room.base[out] = base[at(x - ox, y - oy)];
					room.middle[out] = mid[at(x - ox, y - oy)];
					room.walk[out] = walk[at(x - ox, y - oy)];
				}
				line += inside ? letter(x - ox, y - oy) : '#';
			}
			room.rows.push_back(line);
		}
		room.name = "w_room_" + std::to_string(i);
		room.templateName = chosen.name;
		const auto doorX = building.x0 + building.door;
		const auto doorY = building.y0 + building.doorRow;
		const auto cx = doorX / Chunk;
		const auto cy = doorY / Chunk;
		const auto lx = doorX - cx * Chunk;
		const auto ly = doorY - cy * Chunk;
		const auto ex = exits[0].first + ox;
		const auto ey = exits[0].second + oy;
		m_doorEvents[{ cx, cy }].push_back(std::to_string(lx) + " " + std::to_string(ly) + " 2 Levels/teleport.miniska, " + room.name + ", "
			+ std::to_string(ex) + ", " + std::to_string(ey - 1));
		for (const auto& [x, y] : exits) {
			room.events.push_back(std::to_string(x + ox) + " " + std::to_string(y + oy) + " 2 Levels/teleport.miniska, w_" + std::to_string(cx) + "_"
				+ std::to_string(cy) + ", " + std::to_string(lx) + ", " + std::to_string(ly + 1));
		}
		for (const auto& [x, y] : counter) {
			room.events.push_back(std::to_string(x + ox) + " " + std::to_string(y + oy) + " 1 Levels/pokescript.miniska");
		}
		if (kind == "rest") {
			for (const auto& [x, y] : beds) {
				room.events.push_back(std::to_string(x + ox) + " " + std::to_string(y + oy) + " 1 Levels/rest.miniska");
			}
		}
		m_rooms.push_back(std::move(room));
	}
}

double worldgen::World::top(int x, int y) const {
	if (!inMap(x, y)) {
		return 0.0;
	}
	const auto object = m_objects.find({ x, y });
	return level(x, y) + (object != m_objects.end() ? object->second : 0.0);
}

std::array<std::uint8_t, 3> worldgen::World::cornerLight(int i, int j) const {
	const auto ground = static_cast<double>(level(i - 1, j - 1) + level(i - 1, j) + level(i, j - 1) + level(i, j)) / 4;
	// Occlusion : what stands higher around the corner
	auto ao = 0.0;
	for (auto dy = -3; dy < 3; dy++) {
		for (auto dx = -3; dx < 3; dx++) {
			const auto a = dx + 0.5;
			const auto b = dy + 0.5;
			const auto d = std::sqrt(a * a + b * b); // exact sum of quarters : math.hypot
			if (d < 3.2) {
				ao += std::max(0.0, top(i + dx, j + dy) - ground) / (1 + d * d);
			}
		}
	}
	// Cast shadow : something high up-left of the corner
	auto shadow = 0.0;
	for (auto k = 1; k < 4; k++) {
		if (top(i - k, j - k) - ground > 0.45 * k) {
			shadow = std::max(shadow, 1.0 - (k - 1) * 0.3);
		}
	}
	const auto cloud = Fbm(i / 11.0, j / 11.0, m_seed + 80, 3) - 0.5;
	// Subtle : the deepest shade keeps 82 % of the colour
	auto l = 0.99 - 0.06 * std::min(ao, 1.6) - 0.05 * shadow + 0.01 * ground + 0.05 * cloud;
	l = std::max(0.82, std::min(1.0, l));
	const auto t = (l - 0.82) / 0.18; // 0 in the deepest shade, 1 in the full light
	return { static_cast<std::uint8_t>(static_cast<int>(255 * l * (0.96 + 0.04 * t))),
		static_cast<std::uint8_t>(static_cast<int>(255 * l * (0.98 + 0.02 * t))),
		static_cast<std::uint8_t>(static_cast<int>(255 * l * (1.0 - 0.02 * t))) };
}

std::vector<std::string> worldgen::World::chunkEvents(int cx, int cy) const {
	auto lines = std::vector<std::string>{};
	const auto x0 = cx * Chunk;
	const auto y0 = cy * Chunk;
	const auto chunksX = width() / Chunk;
	const auto chunksY = height() / Chunk;
	const auto walkable = [this](int x, int y) { return inMap(x, y) && walk(x, y); };
	for (auto i = 0; i < Chunk; i++) {
		// (edge block, the block of the neighbour where the player arrives, map, x, y in the neighbour)
		struct Edge { int ex, ey, dx, dy, ncx, ncy, ax, ay; };
		const Edge edges[] = {
			{ 0, i, -2, 0, cx - 1, cy, Chunk - 2, i },
			{ Chunk - 1, i, 2, 0, cx + 1, cy, 1, i },
			{ i, 0, 0, -2, cx, cy - 1, i, Chunk - 2 },
			{ i, Chunk - 1, 0, 2, cx, cy + 1, i, 1 },
		};
		for (const auto& e : edges) {
			if (!(0 <= e.ncx && e.ncx < chunksX && 0 <= e.ncy && e.ncy < chunksY)) {
				continue;
			}
			const auto wx = x0 + e.ex;
			const auto wy = y0 + e.ey;
			if (walkable(wx, wy) && walkable(wx + e.dx, wy + e.dy)) {
				lines.push_back(std::to_string(e.ex) + " " + std::to_string(e.ey) + " 2 Levels/teleport.miniska, w_" + std::to_string(e.ncx) + "_"
					+ std::to_string(e.ncy) + ", " + std::to_string(e.ax) + ", " + std::to_string(e.ay));
			}
		}
	}
	for (const auto* events : { &m_doorEvents, &m_events }) {
		const auto found = events->find({ cx, cy });
		if (found != events->end()) {
			lines.insert(lines.end(), found->second.begin(), found->second.end());
		}
	}
	return lines;
}
