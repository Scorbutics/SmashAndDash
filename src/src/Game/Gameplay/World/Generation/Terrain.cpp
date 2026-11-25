#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <queue>
#include "Terrain.h"
#include "Noise.h"

namespace {
	constexpr double NoCost = -1.0;
	// The lift of each biome (BIOME_LIFT), in the order of the biomes
	constexpr std::array<double, worldgen::BiomeCount> BiomeLift{ 0.0, 0.015, 0.0, 0.05 };

	double Dist(int ax, int ay, int bx, int by) {
		// math.dist of whole numbers : correctly rounded, as the square root of their exact sum
		const auto dx = static_cast<long long>(ax - bx);
		const auto dy = static_cast<long long>(ay - by);
		return std::sqrt(static_cast<double>(dx * dx + dy * dy));
	}

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
}


worldgen::Terrain::Terrain(const WorldPlan& plan) :
	m_plan(plan) {
	planRoutes();
	for (const auto& v : m_plan.villages()) {
		for (const auto* set : { &v.yard, &v.pond, &v.pondRing }) {
			m_villageGround.insert(set->begin(), set->end());
		}
	}
	m_terrain.resize(static_cast<std::size_t>(width() * height()));
	for (auto y = 0; y < height(); y++) {
		for (auto x = 0; x < width(); x++) {
			m_terrain[index(x, y)] = classify(x, y);
		}
	}
	// Until every terrace has its stairs
	for (auto i = 0; i < 8; i++) {
		clean();
		if (!placeStairs()) {
			break;
		}
	}
	planPois();
}

double worldgen::Terrain::heightAt(int x, int y) const {
	const auto& climate = m_plan.climate();
	// A broad landmass, smaller hills and lakes on it (the order of the sums of the Python generator)
	auto h = 0.6 * Fbm(x / 30.0, y / 30.0, climate.seed()) + 0.4 * Fbm(x / 11.0, y / 11.0, climate.seed() + 50, 3);
	// The biomes raise the land a little : the deserts are dry, blended at their borders
	auto lift = 0.0;
	auto total = 0.0;
	for (auto b = 0; b < BiomeCount; b++) {
		const auto w = m_plan.weight(x, y, static_cast<Biome>(b));
		if (w != 0.0) {
			lift += BiomeLift[static_cast<std::size_t>(b)] * w;
			total += w;
		}
	}
	h += lift / total;
	h += 0.06 * climate.relief(x, y);
	const auto border = std::min({ x, y, width() - 1 - x, height() - 1 - y });
	h -= 0.45 * Smooth(std::max(0.0, 1 - border / 10.0));
	const auto found = m_plan.nearVillage().find({ x, y });
	const auto w = found == m_plan.nearVillage().end() ? 0.0 : Smooth(std::max(0.0, std::min(1.0, (7 - found->second) / 5.0)));
	return h * (1 - w) + 0.52 * w;
}

worldgen::Ground worldgen::Terrain::ground(int x, int y) const {
	if (x < 0 || x >= width() || y < 0 || y >= height()) {
		return Ground::Water;
	}
	return m_terrain[index(x, y)].ground;
}

int worldgen::Terrain::level(int x, int y) const {
	if (x < 0 || x >= width() || y < 0 || y >= height()) {
		return 0;
	}
	return m_terrain[index(x, y)].level;
}

bool worldgen::Terrain::inTown(int x, int y, int r) const {
	const auto found = m_plan.nearVillage().find({ x, y });
	return found != m_plan.nearVillage().end() && found->second <= r;
}

void worldgen::Terrain::planRoutes() {
	const auto W = width();
	const auto H = height();
	auto blocked = Blocks{};
	for (const auto& v : m_plan.villages()) {
		blocked.insert(v.footprint.begin(), v.footprint.end());
	}
	auto cost = std::vector<double>(static_cast<std::size_t>(W * H), NoCost);
	for (auto y = 1; y < H - 1; y++) {
		for (auto x = 1; x < W - 1; x++) {
			if (blocked.count({ x, y }) > 0 || heightAt(x, y) < 0.41) {
				continue;
			}
			// A noise : the roads wind, as old paths do
			cost[index(x, y)] = 1.0 + 10.0 * m_plan.climate().relief(x, y) + (m_plan.nearVillage().count({ x, y }) > 0 ? 0.0 : 0.5)
				+ 3.0 * Fbm(x / 6.0, y / 6.0, m_plan.climate().seed() + 500, 2);
		}
	}
	// Prim : the shortest link from the linked villages each time (ties : the first, linked in ascending order)
	const auto& villages = m_plan.villages();
	auto edges = std::vector<std::pair<int, int>>{};
	auto linked = std::set<int>{ 0 };
	while (linked.size() < villages.size()) {
		auto best = std::pair<int, int>{ -1, -1 };
		auto bestDistance = 0.0;
		for (const auto a : linked) {
			for (auto b = 0; b < static_cast<int>(villages.size()); b++) {
				if (linked.count(b) > 0) {
					continue;
				}
				const auto& ca = villages[static_cast<std::size_t>(a)].center;
				const auto& cb = villages[static_cast<std::size_t>(b)].center;
				const auto d = Dist(ca.first, ca.second, cb.first, cb.second);
				if (best.first < 0 || d < bestDistance) {
					best = { a, b };
					bestDistance = d;
				}
			}
		}
		edges.push_back(best);
		linked.insert(best.second);
	}
	for (const auto& [a, b] : edges) {
		auto path = astar(cost, villages[static_cast<std::size_t>(a)].center, villages[static_cast<std::size_t>(b)].center);
		if (!path.has_value()) {
			continue;
		}
		for (auto i = 0u; i < path->size(); i++) {
			const auto [x, y] = (*path)[i];
			m_routes.insert({ x, y });
			const auto [nx, ny] = (*path)[std::min<std::size_t>(i + 1, path->size() - 1)];
			const auto side = nx == x ? Block{ x + 1, y } : Block{ x, y + 1 };
			if (cost[index(side.first, side.second)] != NoCost) {
				m_routes.insert(side);
			}
		}
		m_routePaths.push_back(Route{ a, b, std::move(*path) });
	}
	for (const auto& c : blocked) {
		m_routes.erase(c);
	}
	m_nearRoute = DistanceTo(m_routes, 3);
}

std::optional<std::vector<worldgen::Block>> worldgen::Terrain::astar(const std::vector<double>& cost, Block start, Block goal) const {
	const auto W = width();
	const auto H = height();
	auto best = std::vector<double>(cost.size(), 1e18);
	auto came = std::vector<int>(cost.size(), -1);
	best[index(start.first, start.second)] = 0.0;
	// (f, x, y) : the smallest first, the ties by the block, as the heap of the Python generator
	using Entry = std::tuple<double, int, int>;
	auto heap = std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>>{};
	heap.push(Entry{ 0.0, start.first, start.second });
	while (!heap.empty()) {
		const auto [f, x, y] = heap.top();
		heap.pop();
		if (Block{ x, y } == goal) {
			auto path = std::vector<Block>{ { x, y } };
			auto cur = static_cast<int>(index(x, y));
			while (came[static_cast<std::size_t>(cur)] >= 0) {
				cur = came[static_cast<std::size_t>(cur)];
				path.push_back({ cur % W, cur / W });
			}
			std::reverse(path.begin(), path.end());
			return path;
		}
		for (auto k = 0u; k < 4; k++) {
			const auto nx = x + D8[k][0];
			const auto ny = y + D8[k][1];
			if (nx < 0 || nx >= W || ny < 0 || ny >= H || cost[index(nx, ny)] == NoCost) {
				continue;
			}
			const auto g = best[index(x, y)] + cost[index(nx, ny)];
			if (g < best[index(nx, ny)]) {
				best[index(nx, ny)] = g;
				came[index(nx, ny)] = static_cast<int>(index(x, y));
				heap.push(Entry{ g + std::abs(nx - goal.first) + std::abs(ny - goal.second), nx, ny });
			}
		}
	}
	return std::nullopt;
}

worldgen::Cell worldgen::Terrain::classify(int x, int y) const {
	for (const auto& v : m_plan.villages()) {
		if (v.road.count({ x, y }) > 0 || v.pondRing.count({ x, y }) > 0) {
			return { Ground::Pave, 0 };
		}
		if (v.pond.count({ x, y }) > 0) {
			return { Ground::Water, 0 };
		}
		if (v.yard.count({ x, y }) > 0) {
			return { Ground::Sand, 0 };
		}
	}
	if (m_routes.count({ x, y }) > 0) {
		return { Ground::Path, 0 };
	}
	const auto h = heightAt(x, y);
	if (h < 0.40) {
		return { Ground::Water, 0 };
	}
	if (h < 0.425) {
		return { Ground::Sand, 0 };
	}
	const auto nearRoute = m_nearRoute.find({ x, y });
	if (inTown(x, y, 6) || (nearRoute != m_nearRoute.end() && nearRoute->second <= 2)) {
		return { Ground::Grass, 0 };
	}
	const auto seed = m_plan.climate().seed();
	const auto plateau = Fbm(x / 15.0, y / 15.0, seed + 60, 3) > 0.6 && h > 0.47;
	const auto a = m_plan.climate().relief(x, y);
	const auto m = (h - 0.43) * 14 * a + 1.2 * a * (Fbm(x / 7.0, y / 7.0, seed + 330, 2) - 0.5);
	return { Ground::Grass, std::max(plateau ? 1 : 0, std::min(MaxLevel, static_cast<int>(std::max(0.0, m)))) };
}

std::array<bool, 8> worldgen::Terrain::lowMask(int x, int y) const {
	const auto lv = level(x, y);
	if (lv <= 0) {
		return {};
	}
	return Mask8([this, lv](int a, int b) { return level(a, b) < lv; }, x, y);
}

bool worldgen::Terrain::isFoot(int x, int y) const {
	return 0 <= x && x < width() && 0 <= y && y < height() && ground(x, y) != Ground::Water && level(x, y - 1) > level(x, y);
}

bool worldgen::Terrain::flat(int x, int y) const {
	if (!(0 <= x && x < width() && 0 <= y && y < height()) || ground(x, y) == Ground::Water) {
		return false;
	}
	const auto low = lowMask(x, y);
	return !(low[0] || low[1] || low[2] || low[3]) && !isFoot(x, y);
}

bool worldgen::Terrain::stairOk(int x, int y) const {
	const auto lv = level(x, y);
	const auto low = lowMask(x, y);
	return lv > 0 && low[2] && !low[0] && !low[1] && !low[3] && level(x - 1, y) == lv && level(x + 1, y) == lv
		&& flat(x, y - 1) && isFoot(x, y + 1) && flat(x, y + 2);
}

void worldgen::Terrain::clean() {
	const auto W = width();
	const auto H = height();
	for (auto pass = 0; pass < MaxLevel + 2; pass++) {
		auto changed = false;
		// One block wide : lower on both sides of an axis ; under a face and a cliff itself : no room for both
		for (auto y = 0; y < H; y++) {
			for (auto x = 0; x < W; x++) {
				auto& cell = m_terrain[index(x, y)];
				const auto lv = cell.level;
				if (lv > 0 && ((level(x, y - 1) < lv && level(x, y + 1) < lv) || (level(x - 1, y) < lv && level(x + 1, y) < lv))) {
					cell.level = lv - 1;
					changed = true;
				} else if (lv > 0 && level(x, y - 1) > lv) {
					const auto low = lowMask(x, y);
					if (low[0] || low[1] || low[2] || low[3]) {
						cell.level = lv - 1;
						changed = true;
					}
				}
			}
		}
		// Small patches
		auto seen = std::vector<bool>(m_terrain.size(), false);
		for (auto y = 0; y < H; y++) {
			for (auto x = 0; x < W; x++) {
				if (seen[index(x, y)]) {
					continue;
				}
				const auto kind = m_terrain[index(x, y)];
				auto patch = std::vector<Block>{ { x, y } };
				auto front = patch;
				seen[index(x, y)] = true;
				while (!front.empty()) {
					const auto [a, b] = front.back();
					front.pop_back();
					for (auto k = 0u; k < 4; k++) {
						const auto cx = a + D8[k][0];
						const auto cy = b + D8[k][1];
						if (0 <= cx && cx < W && 0 <= cy && cy < H && !seen[index(cx, cy)] && m_terrain[index(cx, cy)] == kind) {
							seen[index(cx, cy)] = true;
							patch.push_back({ cx, cy });
							front.push_back({ cx, cy });
						}
					}
				}
				if (std::any_of(patch.begin(), patch.end(), [this](const Block& c) { return m_villageGround.count(c) > 0; })) {
					continue;
				}
				auto to = Cell{};
				if (kind.ground == Ground::Grass && kind.level > 0 && patch.size() < 16) {
					to = Cell{ Ground::Grass, kind.level - 1 };
				} else if ((kind.ground == Ground::Sand || kind.ground == Ground::Water) && patch.size() < 5) {
					to = kind.ground != Ground::Water ? Cell{ Ground::Grass, 0 } : Cell{ Ground::Sand, 0 };
				} else {
					continue;
				}
				for (const auto& [a, b] : patch) {
					m_terrain[index(a, b)] = to;
				}
				changed = true;
			}
		}
		if (!changed) {
			break;
		}
	}
}

bool worldgen::Terrain::placeStairs() {
	const auto W = width();
	const auto H = height();
	const auto seed = m_plan.climate().seed();
	m_stairs.clear();
	for (auto y = 0; y < H; y++) {
		for (auto x = 0; x < W; x++) {
			if (x % 9 == 4 && H01(x / 9, y, seed + 5) < 0.5 && stairOk(x, y)) {
				m_stairs.insert({ x, y });
			}
		}
	}
	auto seen = std::vector<bool>(m_terrain.size(), false);
	auto lowered = false;
	for (auto y0 = 0; y0 < H; y0++) {
		for (auto x0 = 0; x0 < W; x0++) {
			const auto lv = level(x0, y0);
			if (lv == 0 || seen[index(x0, y0)]) {
				continue;
			}
			auto region = std::vector<Block>{ { x0, y0 } };
			auto front = region;
			seen[index(x0, y0)] = true;
			while (!front.empty()) {
				const auto [a, b] = front.back();
				front.pop_back();
				for (auto k = 0u; k < 4; k++) {
					const auto cx = a + D8[k][0];
					const auto cy = b + D8[k][1];
					if (0 <= cx && cx < W && 0 <= cy && cy < H && !seen[index(cx, cy)] && level(cx, cy) == lv) {
						seen[index(cx, cy)] = true;
						region.push_back({ cx, cy });
						front.push_back({ cx, cy });
					}
				}
			}
			if (std::none_of(region.begin(), region.end(), [this](const Block& c) { return m_stairs.count(c) > 0; })) {
				auto ok = std::vector<Block>{};
				for (const auto& c : region) {
					if (stairOk(c.first, c.second)) {
						ok.push_back(c);
					}
				}
				std::sort(ok.begin(), ok.end());
				if (!ok.empty()) {
					m_stairs.insert(ok[ok.size() / 2]);
				} else {
					for (const auto& [a, b] : region) {
						m_terrain[index(a, b)].level = lv - 1;
					}
					lowered = true;
				}
			}
		}
	}
	return lowered;
}

bool worldgen::Terrain::poiBlockOk(int x, int y) const {
	if (!(0 <= x && x < width() && 0 <= y && y < height())) {
		return false;
	}
	const auto mx = x % Chunk;
	const auto my = y % Chunk;
	const auto near = m_plan.nearVillage().find({ x, y });
	return 1 < mx && mx < Chunk - 2 && 1 < my && my < Chunk - 2 && m_terrain[index(x, y)] == Cell{ Ground::Grass, 0 } && flat(x, y)
		&& m_routes.count({ x, y }) == 0 && (near == m_plan.nearVillage().end() ? 99 : near->second) > 6 && m_stairs.count({ x, y }) == 0;
}

std::optional<std::tuple<int, int, std::vector<worldgen::Block>>> worldgen::Terrain::poiArea(const std::string& kind, int x, int y, int nx, int ny) const {
	const auto [w, h] = SizeOf(kind);
	auto sides = std::vector<Block>{ { ny - y, nx - x }, { y - ny, x - nx } };
	if (kind == "rest") {
		sides.erase(std::remove(sides.begin(), sides.end(), Block{ 0, 1 }), sides.end());
	}
	for (const auto& [px, py] : sides) {
		for (const auto dist : { 3, 4, 5 }) {
			auto ax = 0, ay = 0;
			if (px == 0) {
				ax = x - w / 2;
				ay = py > 0 ? y + dist : y - dist - h + 1;
			} else if (kind == "rest") {
				ay = y - h + 1;
				ax = px > 0 ? x + dist : x - dist - w + 1;
			} else {
				ay = y - h / 2;
				ax = px > 0 ? x + dist : x - dist - w + 1;
			}
			auto area = std::vector<Block>{};
			for (auto j = 0; j < h; j++) {
				for (auto i = 0; i < w; i++) {
					area.push_back({ ax + i, ay + j });
				}
			}
			auto spur = std::vector<Block>{};
			for (auto k = 1; k < dist; k++) {
				spur.push_back({ x + px * k, y + py * k });
			}
			if (m_routes.count({ x + px, y + py }) > 0) {
				spur.erase(spur.begin());
			}
			if (kind == "rest" && px != 0) {
				if (px < 0) {
					for (auto i = 3; i < w; i++) {
						spur.push_back({ ax + i, ay + h - 1 });
					}
				} else {
					for (auto i = 0; i < 4; i++) {
						spur.push_back({ ax + i, ay + h - 1 });
					}
				}
			}
			const auto ok = std::all_of(area.begin(), area.end(), [this](const Block& c) { return poiBlockOk(c.first, c.second); })
				&& std::all_of(spur.begin(), spur.end(), [this](const Block& c) { return poiBlockOk(c.first, c.second); });
			if (ok) {
				return std::make_tuple(ax, ay, spur);
			}
		}
	}
	return std::nullopt;
}

void worldgen::Terrain::planPois() {
	auto n = std::size_t{ 0 };
	for (const auto& route : m_routePaths) {
		const auto& path = route.path;
		for (auto i = 10; i < static_cast<int>(path.size()) - 10; i += 12) {
			const auto [x, y] = path[static_cast<std::size_t>(i)];
			const auto [nx, ny] = path[static_cast<std::size_t>(i + 1)];
			const auto tooClose = std::any_of(m_pois.begin(), m_pois.end(), [x = x, y = y](const Poi& p) { return Dist(x, y, p.x, p.y) < PoiSpacing; })
				|| std::any_of(m_plan.villages().begin(), m_plan.villages().end(), [x = x, y = y](const Village& v) { return Dist(x, y, v.center.first, v.center.second) < PoiSpacing; });
			if (tooClose) {
				continue;
			}
			auto found = std::optional<std::tuple<int, int, std::vector<Block>>>{};
			auto kind = std::string{};
			for (auto k = 0u; k < PoiKinds.size(); k++) {
				kind = PoiKinds[(n % PoiKinds.size() + k) % PoiKinds.size()];
				found = poiArea(kind, x, y, nx, ny);
				if (found.has_value()) {
					break;
				}
			}
			if (!found.has_value()) {
				continue;
			}
			auto [ax, ay, spur] = *found;
			const auto [w, h] = SizeOf(kind);
			if (kind == "rest" && std::find(spur.begin(), spur.end(), Block{ ax + 3, ay + h - 1 }) == spur.end()) {
				spur.push_back({ ax + 3, ay + h - 1 });
			}
			for (const auto& [cx, cy] : spur) {
				m_terrain[index(cx, cy)] = Cell{ Ground::Path, 0 };
			}
			m_pois.push_back(Poi{ kind, ax, ay, w, h, spur, m_plan.climate().label(ax, ay) });
			n++;
		}
	}
}
