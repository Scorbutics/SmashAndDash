#include <algorithm>
#include <cmath>
#include <tuple>
#include "Region.h"
#include "Noise.h"

namespace {
	using namespace worldgen;

	struct PoiSize { int w, h; };
	PoiSize SizeOf(const std::string& kind) {
		if (kind == "camp") {
			return { 6, 5 };
		}
		if (kind == "ruins") {
			return { 7, 7 };
		}
		return { 6, 6 };
	}
	const std::array<std::string, 3> PoiKinds{ "camp", "ruins", "rest" };

	// math.dist of whole numbers under a whole limit : the exact sum of the squares
	bool Closer(int ax, int ay, int bx, int by, int limit) {
		const auto dx = static_cast<long long>(ax - bx);
		const auto dy = static_cast<long long>(ay - by);
		return dx * dx + dy * dy < static_cast<long long>(limit) * limit;
	}
}

worldgen::RegionBuilder::RegionBuilder(Infinite& world, int rx, int ry, int margin, int plan) :
	m_rx(rx), m_ry(ry) {
	const auto gx = rx * Region;
	const auto gy = ry * Region;
	m_options.x0 = gx - margin;
	m_options.y0 = gy - margin;
	m_options.width = Region + 2 * margin;
	m_options.height = Region + 2 * margin;
	for (auto j = 0; j < RegionChunks; j++) {
		for (auto i = 0; i < RegionChunks; i++) {
			m_options.chunks.push_back({ rx * RegionChunks + i, ry * RegionChunks + j });
		}
	}
	m_options.regionX0 = gx;
	m_options.regionY0 = gy;
	m_options.regionSize = Region;

	// The terrain (cleaned, its stairs) of the plan window (the reefs : water, until the tiles of the ocean)
	const auto pm = margin + plan;
	const auto p0 = gx - pm;
	const auto q0 = gy - pm;
	const auto pw = Region + 2 * pm;
	const auto area = world.area(p0, q0, p0 + pw, q0 + pw);
	m_plan = std::unique_ptr<WorldPlan>(new WorldPlan(world.seed()));
	m_terrain = std::unique_ptr<Terrain>(new Terrain(*m_plan, p0, q0, pw, pw));
	auto& terrain = *m_terrain;
	terrain.m_terrain = area.terrain;
	for (auto& c : terrain.m_terrain) {
		if (c.ground == Ground::Reef) {
			m_reefs++;
			c = Cell{ Ground::Water, 0 };
		}
	}
	terrain.m_stairs.insert(area.stairs.begin(), area.stairs.end());

	// The villages of the cells of the plan window and around (the other ends of its roads)
	auto& p = *m_plan;
	const auto c0 = FloorDiv(p0, VillageCell) - RoadCells;
	const auto c1 = FloorDiv(p0 + pw, VillageCell) + RoadCells;
	const auto d0 = FloorDiv(q0, VillageCell) - RoadCells;
	const auto d1 = FloorDiv(q0 + pw, VillageCell) + RoadCells;
	auto index = std::map<Cell2, int>{};
	for (auto cj = d0; cj <= d1; cj++) {
		for (auto ci = c0; ci <= c1; ci++) {
			if (const auto* v = world.village(ci, cj)) {
				index[{ ci, cj }] = static_cast<int>(p.m_villages.size());
				p.m_villages.push_back(v->v);
				m_cells.push_back({ ci, cj });
			}
		}
	}
	for (const auto& cell : m_cells) {
		for (const auto& [c, d] : world.village(cell.first, cell.second)->nearBiome) {
			if (d > 8) {
				continue;
			}
			const auto found = p.m_nearVillage.find(c);
			if (found == p.m_nearVillage.end() || d < found->second) {
				p.m_nearVillage[c] = d;
			}
		}
	}

	// The roads that reach the plan window
	const auto pc0 = FloorDiv(p0, VillageCell);
	const auto pc1 = FloorDiv(p0 + pw, VillageCell);
	const auto pd0 = FloorDiv(q0, VillageCell);
	const auto pd1 = FloorDiv(q0 + pw, VillageCell);
	for (const auto& [ca, cb] : world.roads(pc0 - RoadCells, pd0 - RoadCells, pc1 + RoadCells, pd1 + RoadCells)) {
		const auto box = world.roadBox(world.village(ca.first, ca.second)->v, world.village(cb.first, cb.second)->v);
		if (box[2] < p0 || box[0] >= p0 + pw || box[3] < q0 || box[1] >= q0 + pw) {
			continue;
		}
		const auto* blocks = world.road(ca, cb);
		if (blocks == nullptr) {
			continue;
		}
		terrain.m_routes.insert(blocks->begin(), blocks->end());
		const auto from = index.find(ca);
		const auto to = index.find(cb);
		terrain.m_routePaths.push_back(Route{ from == index.end() ? -1 : from->second, to == index.end() ? -1 : to->second, *world.roadPath(ca, cb) });
	}
	terrain.m_nearRoute = DistanceTo(terrain.m_routes, 3);
	biomeField(world);
	planPois(world);

	// The sea of the blocks built : the reefs (water, a rock on it), the deep water
	m_options.sea.assign(static_cast<std::size_t>(m_options.width) * static_cast<std::size_t>(m_options.height), RegionOptions::SeaNone);
	for (auto y = m_options.y0; y < m_options.y0 + m_options.height; y++) {
		for (auto x = m_options.x0; x < m_options.x0 + m_options.width; x++) {
			if (terrain.ground(x, y) != Ground::Water) {
				continue;
			}
			auto& sea = m_options.sea[static_cast<std::size_t>(y - m_options.y0) * static_cast<std::size_t>(m_options.width) + static_cast<std::size_t>(x - m_options.x0)];
			if (area.at(x, y).ground == Ground::Reef) {
				sea = RegionOptions::SeaReef;
			} else if (world.ocean(x, y) == Sea::Deep) {
				sea = RegionOptions::SeaDeep;
			}
		}
	}
	// The chunks mostly sea (every other block)
	for (const auto& [cx, cy] : m_options.chunks) {
		auto sea = 0;
		for (auto y = cy * Chunk; y < cy * Chunk + Chunk; y += 2) {
			for (auto x = cx * Chunk; x < cx * Chunk + Chunk; x += 2) {
				sea += world.ocean(x, y) != Sea::None ? 1 : 0;
			}
		}
		if (2 * sea > (Chunk / 2) * (Chunk / 2)) {
			m_plan->m_seaChunks.insert({ cx, cy });
		}
	}
}

worldgen::RegionBuilder::~RegionBuilder() = default;

void worldgen::RegionBuilder::biomeField(Infinite& world) {
	// worldgen.biome_field : the biomes blurred (two box blurs of 11 blocks), in integers (the sums of the blocks around :
	// the same in any window), the biomes of the corners ; the biome of each chunk : the most of its blocks
	auto& p = *m_plan;
	const auto W = m_options.width;
	const auto H = m_options.height;
	constexpr auto m = 10;
	const auto x0 = m_options.x0 - m;
	const auto y0 = m_options.y0 - m;
	const auto n = W + 2 * m;
	auto label = std::vector<Biome>(static_cast<std::size_t>(n) * static_cast<std::size_t>(n));
	for (auto j = 0; j < n; j++) {
		for (auto i = 0; i < n; i++) {
			label[static_cast<std::size_t>(j) * static_cast<std::size_t>(n) + static_cast<std::size_t>(i)] = world.biome(x0 + i, y0 + j);
		}
	}
	p.m_x0 = m_options.x0;
	p.m_y0 = m_options.y0;
	p.m_width = W;
	p.m_height = H;
	p.m_weights.assign(static_cast<std::size_t>(W) * static_cast<std::size_t>(H) * BiomeCount, 0.0);
	// A grid of width x height (by rows) summed over 11 blocks along its rows : width - 10 columns
	const auto boxRows = [](const std::vector<long long>& g, int width, int height) {
		auto out = std::vector<long long>(static_cast<std::size_t>(width - 10) * static_cast<std::size_t>(height));
		auto pre = std::vector<long long>(static_cast<std::size_t>(width) + 1);
		for (auto j = 0; j < height; j++) {
			pre[0] = 0;
			for (auto i = 0; i < width; i++) {
				pre[static_cast<std::size_t>(i) + 1] = pre[static_cast<std::size_t>(i)] + g[static_cast<std::size_t>(j) * static_cast<std::size_t>(width) + static_cast<std::size_t>(i)];
			}
			for (auto i = 0; i < width - 10; i++) {
				out[static_cast<std::size_t>(j) * static_cast<std::size_t>(width - 10) + static_cast<std::size_t>(i)] = pre[static_cast<std::size_t>(i) + 11] - pre[static_cast<std::size_t>(i)];
			}
		}
		return out;
	};
	const auto boxColumns = [](const std::vector<long long>& g, int width, int height) {
		auto out = std::vector<long long>(static_cast<std::size_t>(width) * static_cast<std::size_t>(height - 10));
		auto pre = std::vector<long long>(static_cast<std::size_t>(height) + 1);
		for (auto i = 0; i < width; i++) {
			pre[0] = 0;
			for (auto j = 0; j < height; j++) {
				pre[static_cast<std::size_t>(j) + 1] = pre[static_cast<std::size_t>(j)] + g[static_cast<std::size_t>(j) * static_cast<std::size_t>(width) + static_cast<std::size_t>(i)];
			}
			for (auto j = 0; j < height - 10; j++) {
				out[static_cast<std::size_t>(j) * static_cast<std::size_t>(width) + static_cast<std::size_t>(i)] = pre[static_cast<std::size_t>(j) + 11] - pre[static_cast<std::size_t>(j)];
			}
		}
		return out;
	};
	constexpr auto full = 11.0 * 11 * 11 * 11;
	for (auto b = 0; b < BiomeCount; b++) {
		auto g = std::vector<long long>(label.size());
		for (auto k = 0u; k < label.size(); k++) {
			g[k] = label[k] == static_cast<Biome>(b) ? 1 : 0;
		}
		g = boxRows(g, n, n);
		g = boxColumns(g, n - 10, n);
		g = boxRows(g, n - 10, n - 10);
		g = boxColumns(g, n - 20, n - 10);
		for (auto j = 0; j < H; j++) {
			for (auto i = 0; i < W; i++) {
				const auto v = g[static_cast<std::size_t>(j) * static_cast<std::size_t>(W) + static_cast<std::size_t>(i)];
				if (static_cast<double>(v) > 0.001 * full) {
					p.m_weights[(static_cast<std::size_t>(j) * static_cast<std::size_t>(W) + static_cast<std::size_t>(i)) * BiomeCount + static_cast<std::size_t>(b)] = static_cast<double>(v) / full;
				}
			}
		}
	}
	p.m_corners.assign(static_cast<std::size_t>(W + 1) * static_cast<std::size_t>(H + 1), {});
	for (auto j = 0; j <= H; j++) {
		for (auto i = 0; i <= W; i++) {
			auto acc = std::array<double, BiomeCount>{};
			for (const auto dx : { -1, 0 }) {
				for (const auto dy : { -1, 0 }) {
					const auto bx = std::min(W - 1, std::max(0, i + dx));
					const auto by = std::min(H - 1, std::max(0, j + dy));
					for (auto k = 0; k < BiomeCount; k++) {
						acc[static_cast<std::size_t>(k)] += p.m_weights[(static_cast<std::size_t>(by) * static_cast<std::size_t>(W) + static_cast<std::size_t>(bx)) * BiomeCount + static_cast<std::size_t>(k)];
					}
				}
			}
			auto total = 0.0;
			for (const auto a : acc) {
				total += a;
			}
			auto raw = std::array<double, BiomeCount>{};
			auto q = std::array<int, BiomeCount>{};
			auto sum = 0;
			for (auto k = 0; k < BiomeCount; k++) {
				raw[static_cast<std::size_t>(k)] = acc[static_cast<std::size_t>(k)] / total * MixSteps;
				q[static_cast<std::size_t>(k)] = static_cast<int>(raw[static_cast<std::size_t>(k)]);
				sum += q[static_cast<std::size_t>(k)];
			}
			auto order = std::array<int, BiomeCount>{ 0, 1, 2, 3 };
			std::stable_sort(order.begin(), order.end(), [&](int a, int b) {
				return -(raw[static_cast<std::size_t>(a)] - q[static_cast<std::size_t>(a)]) < -(raw[static_cast<std::size_t>(b)] - q[static_cast<std::size_t>(b)]);
			});
			for (auto k = 0; k < MixSteps - sum && k < BiomeCount; k++) {
				q[static_cast<std::size_t>(order[static_cast<std::size_t>(k)])]++;
			}
			auto& c = p.m_corners[static_cast<std::size_t>(j) * static_cast<std::size_t>(W + 1) + static_cast<std::size_t>(i)];
			for (auto k = 0; k < BiomeCount; k++) {
				c[static_cast<std::size_t>(k)] = static_cast<std::uint8_t>(q[static_cast<std::size_t>(k)]);
			}
		}
	}
	// The biome of each chunk : the most of its blocks (every other block), a tie to the first by name
	for (const auto& [cx, cy] : m_options.chunks) {
		auto count = std::array<int, BiomeCount>{};
		for (auto y = cy * Chunk; y < cy * Chunk + Chunk; y += 2) {
			for (auto x = cx * Chunk; x < cx * Chunk + Chunk; x += 2) {
				count[static_cast<std::size_t>(world.biome(x, y))]++;
			}
		}
		auto best = Biome::Meadow;
		auto bestCount = -1;
		for (const auto b : { Biome::Autumn, Biome::Desert, Biome::Meadow, Biome::Snow }) {
			const auto c = count[static_cast<std::size_t>(b)];
			if (c > 0 && c > bestCount) {
				best = b;
				bestCount = c;
			}
		}
		p.m_chunkBiomes[{ cx, cy }] = best;
	}
}

void worldgen::RegionBuilder::planPois(Infinite& world) {
	// worldgen.plan_pois, local : the places tried every 12 blocks of each road (the kind to try first from a hash) ;
	// one is kept when no other found within PoiSpacing has a higher priority (a hash), and none is within PoiSpacing of
	// a village ; its path from the road paved
	auto& terrain = *m_terrain;
	const auto seed = world.seed();
	const auto& o = m_options;
	const std::array<int, 4> reach{ o.x0 - 12, o.y0 - 12, o.x0 + o.width + 12, o.y0 + o.height + 12 };
	const std::array<int, 4> span{ reach[0] - PoiSpacing, reach[1] - PoiSpacing, reach[2] + PoiSpacing, reach[3] + PoiSpacing };
	struct Found {
		std::tuple<double, int, int> priority;
		std::string kind;
		int x, y;
		int ax, ay;
		std::vector<Block> spur;
		bool operator<(const Found& other) const {
			return std::tie(priority, kind, x, y, ax, ay, spur) < std::tie(other.priority, other.kind, other.x, other.y, other.ax, other.ay, other.spur);
		}
	};
	auto found = std::vector<Found>{};
	const auto& villages = m_plan->villages();
	for (const auto& route : terrain.m_routePaths) {
		const auto& path = route.path;
		for (auto i = 10; i < static_cast<int>(path.size()) - 10; i += 12) {
			const auto [x, y] = path[static_cast<std::size_t>(i)];
			const auto [nx, ny] = path[static_cast<std::size_t>(i + 1)];
			if (!(span[0] <= x && x < span[2] && span[1] <= y && y < span[3])) {
				continue;
			}
			if (std::any_of(villages.begin(), villages.end(), [x = x, y = y](const Village& v) { return Closer(x, y, v.center.first, v.center.second, PoiSpacing); })) {
				continue;
			}
			const auto first = static_cast<std::size_t>(H01(x, y, seed + 43) * static_cast<double>(PoiKinds.size()));
			for (auto k = 0u; k < PoiKinds.size(); k++) {
				const auto& kind = PoiKinds[(first + k) % PoiKinds.size()];
				if (auto got = terrain.poiArea(kind, x, y, nx, ny)) {
					auto& [ax, ay, spur] = *got;
					found.push_back(Found{ { H01(x, y, seed + 44), y, x }, kind, x, y, ax, ay, std::move(spur) });
					break;
				}
			}
		}
	}
	auto sorted = found;
	std::sort(sorted.begin(), sorted.end());
	for (auto& f : sorted) {
		if (!(reach[0] <= f.x && f.x < reach[2] && reach[1] <= f.y && f.y < reach[3])) {
			continue;
		}
		if (std::any_of(found.begin(), found.end(), [&f](const Found& other) { return other.priority > f.priority && Closer(f.x, f.y, other.x, other.y, PoiSpacing); })) {
			continue;
		}
		const auto [w, h] = SizeOf(f.kind);
		auto spur = f.spur;
		if (f.kind == "rest" && std::find(spur.begin(), spur.end(), Block{ f.ax + 3, f.ay + h - 1 }) == spur.end()) {
			spur.push_back({ f.ax + 3, f.ay + h - 1 });
		}
		for (const auto& [cx, cy] : spur) {
			if (terrain.inside(cx, cy)) {
				terrain.m_terrain[terrain.index(cx, cy)] = Cell{ Ground::Path, 0 };
			}
		}
		terrain.m_pois.push_back(Poi{ f.kind, f.ax, f.ay, w, h, spur, world.biome(f.ax, f.ay) });
	}
}

std::unique_ptr<worldgen::World> worldgen::RegionBuilder::build(const Shapes& shapes, const std::vector<RoomTemplate>& templates) const {
	return std::make_unique<World>(*m_terrain, shapes, templates, &m_options);
}
