#include <algorithm>
#include <cmath>
#include <queue>
#include <string>
#include <tuple>
#include "Infinite.h"
#include "Noise.h"

namespace {
	using namespace worldgen;

	// The distribution of fbm (3 and 4 octaves) at each 1/32 of its cumulative distribution (infinite.py RANK3, RANK4)
	constexpr std::array<double, 33> Rank3Table{ 0.033, 0.2443, 0.2831, 0.3106, 0.3322, 0.3505, 0.3669, 0.3824, 0.3973, 0.4116, 0.4246, 0.4372, 0.4498, 0.4623,
		0.4748, 0.4867, 0.4989, 0.5109, 0.5226, 0.5347, 0.5472, 0.56, 0.5732, 0.5865, 0.6006, 0.6148, 0.6302, 0.6466,
		0.6656, 0.6874, 0.7144, 0.7532, 0.9573 };
	constexpr std::array<double, 33> Rank4Table{ 0.0539, 0.2602, 0.2967, 0.3222, 0.3425, 0.3609, 0.3762, 0.3904, 0.4044, 0.4173, 0.4295, 0.4414, 0.4534, 0.4654,
		0.4769, 0.4885, 0.4997, 0.5111, 0.5227, 0.5341, 0.5458, 0.5576, 0.5697, 0.5823, 0.5953, 0.6091, 0.6236, 0.6398,
		0.6573, 0.6785, 0.7032, 0.7399, 0.9452 };

	double Rank(const std::array<double, 33>& table, double v) {
		if (v <= table.front()) {
			return 0.0;
		}
		if (v >= table.back()) {
			return 1.0;
		}
		auto lo = std::size_t{ 0 };
		auto hi = table.size() - 1;
		while (hi - lo > 1) {
			const auto mid = (lo + hi) / 2;
			if (table[mid] <= v) {
				lo = mid;
			} else {
				hi = mid;
			}
		}
		return (static_cast<double>(lo) + (v - table[lo]) / (table[hi] - table[lo])) / static_cast<double>(table.size() - 1);
	}

	constexpr double ContinentScale = 520.0;
	constexpr double Ocean = 0.34;
	constexpr double Coast = 0.1;
	constexpr int BiomeStep = 4;
	constexpr int BiomeFilter = 8;
	constexpr double Cold = 0.22, Hot = 0.78, Wet = 0.55;
	constexpr int VillageMargin = 40;
	constexpr double VillageLand = 0.7, VillageChance = 0.85, VillageShare = 0.6;
	constexpr int VillageTries = 3;
	constexpr int VillageLevel = 8, VillageBiome = 12;
	constexpr int StartSearch = 12;
	constexpr std::size_t RoadNearest = 2;
	constexpr int RoadReach = 300, RoadMargin = 36, RoadEdge = 12;
	// The passes over the terrain and how far they reach (infinite.py)
	constexpr int CleanPasses = 2, StairRounds = 1, TerraceOpen = 32;
	constexpr int CleanReach = 17, StairReach = TerraceOpen + 3;
	constexpr int Apron = CleanPasses * CleanReach + StairRounds * (StairReach + CleanReach) + StairReach;
	constexpr std::array<double, BiomeCount> BiomeLift{ 0.0, 0.015, 0.0, 0.05 };

	Biome InfiniteClimateBiome(double t, double h) {
		if (t < Cold) {
			return Biome::Snow;
		}
		if (t > Hot) {
			return Biome::Desert;
		}
		return h > Wet ? Biome::Autumn : Biome::Meadow;
	}

	bool ByName(Biome a, Biome b) {
		return std::string{ BiomeName(a) } < std::string{ BiomeName(b) };
	}

	double Clamp01(double v) {
		return std::max(0.0, std::min(1.0, v));
	}

	// worldgen.World.astar over a cost function (a negative cost : not crossed) : 4 neighbours, the Manhattan heuristic,
	// the ties of the heap broken by the block
	template <class Cost>
	std::optional<std::vector<Block>> AStar(Cost&& cost, Block start, Block goal) {
		auto best = std::unordered_map<std::uint64_t, double>{};
		auto came = std::unordered_map<std::uint64_t, Block>{};
		best[Key(start.first, start.second)] = 0.0;
		using Entry = std::tuple<double, int, int>;
		auto heap = std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>>{};
		heap.push(Entry{ 0.0, start.first, start.second });
		while (!heap.empty()) {
			const auto [f, x, y] = heap.top();
			heap.pop();
			if (Block{ x, y } == goal) {
				auto path = std::vector<Block>{ { x, y } };
				auto cur = Block{ x, y };
				for (auto found = came.find(Key(x, y)); found != came.end(); found = came.find(Key(cur.first, cur.second))) {
					cur = found->second;
					path.push_back(cur);
				}
				std::reverse(path.begin(), path.end());
				return path;
			}
			const auto g0 = best[Key(x, y)];
			for (auto k = 0u; k < 4; k++) {
				const auto nx = x + D8[k][0];
				const auto ny = y + D8[k][1];
				const auto c = cost(nx, ny);
				if (c < 0.0) {
					continue;
				}
				const auto g = g0 + c;
				const auto key = Key(nx, ny);
				const auto known = best.find(key);
				if (g < (known == best.end() ? 1e18 : known->second)) {
					best[key] = g;
					came[key] = Block{ x, y };
					heap.push(Entry{ g + std::abs(nx - goal.first) + std::abs(ny - goal.second), nx, ny });
				}
			}
		}
		return std::nullopt;
	}

	// The terrain of a window cleaned and its stairs placed (infinite.py Window) : right inside, Apron blocks from its
	// edges (the blocks beyond are read as the sea)
	class Window {
	public:
		Window(Infinite& world, int x0, int y0, int x1, int y1) :
			m_world(world), m_x0(x0), m_y0(y0), m_w(x1 - x0), m_h(y1 - y0),
			m_t(static_cast<std::size_t>(m_w) * static_cast<std::size_t>(m_h)),
			m_ground(m_t.size(), 0) {
			for (auto j = 0; j < m_h; j++) {
				for (auto i = 0; i < m_w; i++) {
					m_t[idx(i, j)] = world.classify(x0 + i, y0 + j);
				}
			}
			// The yards and the ponds of the villages stay
			for (auto cj = FloorDiv(y0, VillageCell); cj <= FloorDiv(y1 - 1, VillageCell); cj++) {
				for (auto ci = FloorDiv(x0, VillageCell); ci <= FloorDiv(x1 - 1, VillageCell); ci++) {
					const auto* v = world.village(ci, cj);
					if (v == nullptr) {
						continue;
					}
					for (const auto* set : { &v->v.yard, &v->v.pond, &v->v.pondRing }) {
						for (const auto& [x, y] : *set) {
							if (in(x - x0, y - y0)) {
								m_ground[idx(x - x0, y - y0)] = 1;
							}
						}
					}
				}
			}
			for (auto p = 0; p < CleanPasses; p++) {
				clean();
			}
			for (auto r = 0; r < StairRounds; r++) {
				placeStairs(true);
				clean();
			}
			m_unreached = placeStairs(false);
		}

		int width() const { return m_w; }
		int height() const { return m_h; }
		const Cell& t(int i, int j) const { return m_t[idx(i, j)]; }
		const std::vector<char>& stairs() const { return m_stairs; }
		const std::vector<std::vector<Block>>& unreached() const { return m_unreached; }

	private:
		std::size_t idx(int i, int j) const { return static_cast<std::size_t>(j) * static_cast<std::size_t>(m_w) + static_cast<std::size_t>(i); }
		bool in(int i, int j) const { return 0 <= i && i < m_w && 0 <= j && j < m_h; }
		Ground mat(int i, int j) const { return in(i, j) ? m_t[idx(i, j)].ground : Ground::Water; }
		int level(int i, int j) const { return in(i, j) ? m_t[idx(i, j)].level : 0; }

		std::array<bool, 4> low4(int i, int j) const {
			const auto lv = level(i, j);
			auto out = std::array<bool, 4>{};
			for (auto k = 0u; k < 4; k++) {
				out[k] = lv > 0 && level(i + D8[k][0], j + D8[k][1]) < lv;
			}
			return out;
		}
		static bool any(const std::array<bool, 4>& bits) { return bits[0] || bits[1] || bits[2] || bits[3]; }
		bool isFoot(int i, int j) const { return in(i, j) && mat(i, j) != Ground::Water && level(i, j - 1) > level(i, j); }
		bool flat(int i, int j) const { return in(i, j) && mat(i, j) != Ground::Water && !any(low4(i, j)) && !isFoot(i, j); }
		bool stairOk(int i, int j) const {
			const auto lv = level(i, j);
			const auto low = low4(i, j);
			return lv > 0 && low[2] && !low[0] && !low[1] && !low[3] && level(i - 1, j) == lv && level(i + 1, j) == lv
				&& flat(i, j - 1) && isFoot(i, j + 1) && flat(i, j + 2);
		}

		// The patches of blocks (4 neighbours) where same links two blocks, by their first block in the order of the rows
		template <class Same>
		std::vector<std::vector<Block>> patches(Same&& same) const {
			auto seen = std::vector<char>(m_t.size(), 0);
			auto out = std::vector<std::vector<Block>>{};
			auto front = std::vector<Block>{};
			for (auto j0 = 0; j0 < m_h; j0++) {
				for (auto i0 = 0; i0 < m_w; i0++) {
					if (seen[idx(i0, j0)]) {
						continue;
					}
					seen[idx(i0, j0)] = 1;
					auto patch = std::vector<Block>{ { i0, j0 } };
					front.assign(1, Block{ i0, j0 });
					while (!front.empty()) {
						const auto [a, b] = front.back();
						front.pop_back();
						for (auto k = 0u; k < 4; k++) {
							const auto i = a + D8[k][0];
							const auto j = b + D8[k][1];
							if (in(i, j) && !seen[idx(i, j)] && same(m_t[idx(a, b)], m_t[idx(i, j)])) {
								seen[idx(i, j)] = 1;
								patch.push_back({ i, j });
								front.push_back({ i, j });
							}
						}
					}
					out.push_back(std::move(patch));
				}
			}
			return out;
		}

		void clean() {
			auto next = m_t;
			for (auto j = 0; j < m_h; j++) {
				for (auto i = 0; i < m_w; i++) {
					const auto lv = m_t[idx(i, j)].level;
					if (lv > 0 && ((level(i, j - 1) < lv && level(i, j + 1) < lv) || (level(i - 1, j) < lv && level(i + 1, j) < lv)
						|| (level(i, j - 1) > lv && any(low4(i, j))))) {
						next[idx(i, j)].level = lv - 1;
					}
				}
			}
			m_t = std::move(next);
			for (const auto& patch : patches([](const Cell& a, const Cell& b) { return a == b; })) {
				const auto kind = m_t[idx(patch[0].first, patch[0].second)];
				if (std::any_of(patch.begin(), patch.end(), [this](const Block& c) { return m_ground[idx(c.first, c.second)] != 0; })) {
					continue;
				}
				auto to = Cell{};
				if (kind.ground == Ground::Grass && kind.level > 0 && patch.size() < 16) {
					to = Cell{ Ground::Grass, kind.level - 1 };
				} else if (kind.ground == Ground::Sand && patch.size() < 5) {
					const auto bar = m_world.ocean(m_x0 + patch[0].first, m_y0 + patch[0].second) == Sea::Bar;
					to = bar ? Cell{ Ground::Water, 0 } : Cell{ Ground::Grass, 0 };
				} else if (kind.ground == Ground::Water && patch.size() < 5) {
					to = Cell{ Ground::Sand, 0 };
				} else {
					continue;
				}
				for (const auto& [i, j] : patch) {
					m_t[idx(i, j)] = to;
				}
			}
		}

		std::vector<std::vector<Block>> placeStairs(bool lower) {
			const auto seed = m_world.seed();
			m_stairs.assign(m_t.size(), 0);
			for (auto j = 0; j < m_h; j++) {
				for (auto i = 0; i < m_w; i++) {
					if (FloorMod(m_x0 + i, 9) == 4 && H01(FloorDiv(m_x0 + i, 9), m_y0 + j, seed + 5) < 0.5 && stairOk(i, j)) {
						m_stairs[idx(i, j)] = 1;
					}
				}
			}
			auto unreached = std::vector<std::vector<Block>>{};
			for (auto& patch : patches([](const Cell& a, const Cell& b) { return a.level == b.level; })) {
				const auto lv = level(patch[0].first, patch[0].second);
				if (lv == 0) {
					continue;
				}
				auto minX = m_w, maxX = -1, minY = m_h, maxY = -1;
				for (const auto& [i, j] : patch) {
					minX = std::min(minX, i);
					maxX = std::max(maxX, i);
					minY = std::min(minY, j);
					maxY = std::max(maxY, j);
				}
				if (maxX - minX >= TerraceOpen || maxY - minY >= TerraceOpen || minX == 0 || minY == 0 || maxX == m_w - 1 || maxY == m_h - 1) {
					continue;
				}
				if (std::any_of(patch.begin(), patch.end(), [this](const Block& c) { return m_stairs[idx(c.first, c.second)] != 0; })) {
					continue;
				}
				auto ok = std::vector<Block>{};
				for (const auto& [i, j] : patch) {
					if (stairOk(i, j)) {
						ok.push_back({ j, i });
					}
				}
				if (!ok.empty()) {
					std::sort(ok.begin(), ok.end());
					const auto [j, i] = ok[ok.size() / 2];
					m_stairs[idx(i, j)] = 1;
				} else if (lower) {
					for (const auto& [i, j] : patch) {
						m_t[idx(i, j)].level = lv - 1;
					}
				} else {
					unreached.push_back(std::move(patch));
				}
			}
			return unreached;
		}

		Infinite& m_world;
		int m_x0, m_y0, m_w, m_h;
		std::vector<Cell> m_t;
		std::vector<char> m_ground;
		std::vector<char> m_stairs;
		std::vector<std::vector<Block>> m_unreached;
	};
}

double worldgen::Rank3(double v) {
	return Rank(Rank3Table, v);
}

double worldgen::Rank4(double v) {
	return Rank(Rank4Table, v);
}

std::int64_t worldgen::CellSeed(std::int64_t seed, std::int64_t i, std::int64_t j) {
	const auto ui = static_cast<std::uint64_t>(i);
	const auto uj = static_cast<std::uint64_t>(j);
	const auto us = static_cast<std::uint64_t>(seed);
	auto n = ((ui * 73856093ULL) ^ (uj * 19349663ULL) ^ (us * 83492791ULL)) & 0xFFFFFFFFULL;
	n = ((n ^ (n >> 15)) * 2246822519ULL) & 0xFFFFFFFFULL;
	return static_cast<std::int64_t>((n ^ (n >> 13)) & 0x7FFFFFFFULL);
}

worldgen::Infinite::Infinite(std::int64_t seed) :
	m_seed(seed),
	m_ox(H01(seed, 1, 301) * 500),
	m_oy(H01(seed, 2, 301) * 500) {
}

worldgen::Infinite::~Infinite() = default;

// ---------------------------------------------------------------- the climate

double worldgen::Infinite::continent(int x, int y) const {
	const auto c = Rank4(Fbm((x + m_ox) / ContinentScale, (y + m_oy) / ContinentScale, m_seed + 500, 4));
	return Smooth(Clamp01((c - Ocean) / Coast));
}

double worldgen::Infinite::relief(int x, int y) const {
	const auto r = Rank3(Fbm((x + m_ox) / ClimateScale, (y + m_oy) / ClimateScale, m_seed + 320, 3));
	return Smooth(Clamp01((r - ReliefFrom) / (ReliefFull - ReliefFrom)));
}

worldgen::Biome worldgen::Infinite::climateAt(int x, int y) {
	const auto key = Key(x, y);
	if (const auto found = m_climate.find(key); found != m_climate.end()) {
		return found->second;
	}
	const auto s = m_seed;
	const auto wx = x + 12 * (Fbm(x / 30.0, y / 30.0, s + 404, 2) - 0.5) / 0.25 + 4 * (Fbm(x / 9.0, y / 9.0, s + 400, 3) - 0.5) / 0.25;
	const auto wy = y + 12 * (Fbm(x / 30.0, y / 30.0, s + 405, 2) - 0.5) / 0.25 + 4 * (Fbm(x / 9.0, y / 9.0, s + 401, 3) - 0.5) / 0.25;
	const auto t = Rank3(Fbm((wx + m_ox) / ClimateScale, (wy + m_oy) / ClimateScale, s + 300, 3));
	const auto h = Rank3(Fbm((wx + m_ox) / ClimateScale, (wy + m_oy) / ClimateScale, s + 310, 3));
	const auto biome = InfiniteClimateBiome(t, h);
	m_climate.emplace(key, biome);
	return biome;
}

worldgen::Biome worldgen::Infinite::biome(int x, int y) {
	const auto* v = villageOf(x, y);
	if (v != nullptr && v->nearBiomeFast.count(Key(x, y)) > 0) {
		return v->v.biome;
	}
	return baseBiome(x, y);
}

worldgen::Biome worldgen::Infinite::baseBiome(int x, int y) {
	const auto gx = FloorDiv(x, BiomeStep) * BiomeStep;
	const auto gy = FloorDiv(y, BiomeStep) * BiomeStep;
	const auto key = Key(gx, gy);
	if (const auto found = m_base.find(key); found != m_base.end()) {
		return found->second;
	}
	auto count = std::array<int, BiomeCount>{};
	constexpr auto r = BiomeFilter / BiomeStep;
	for (auto j = -r; j <= r; j++) {
		for (auto i = -r; i <= r; i++) {
			if (i * i + j * j > r * r) {
				continue;
			}
			count[static_cast<std::size_t>(climateAt(gx + i * BiomeStep, gy + j * BiomeStep))]++;
		}
	}
	const auto best = *std::max_element(count.begin(), count.end());
	const auto own = climateAt(gx, gy);
	auto biome = own;
	if (count[static_cast<std::size_t>(own)] != best) {
		auto found = false;
		for (auto b = 0; b < BiomeCount; b++) {
			const auto candidate = static_cast<Biome>(b);
			if (count[static_cast<std::size_t>(b)] == best && (!found || ByName(candidate, biome))) {
				biome = candidate;
				found = true;
			}
		}
	}
	m_base.emplace(key, biome);
	return biome;
}

// ---------------------------------------------------------------- the ocean, the height

double worldgen::Infinite::height(int x, int y) {
	const auto h = landHeight(x, y);
	const auto* v = villageOf(x, y);
	if (v == nullptr) {
		return h;
	}
	const auto found = v->near.find(Key(x, y));
	if (found == v->near.end()) {
		return h;
	}
	const auto k = Smooth(Clamp01((7 - found->second) / 5.0));
	return h * (1 - k) + 0.52 * k;
}

double worldgen::Infinite::landHeight(int x, int y) {
	const auto s = m_seed;
	auto h = 0.6 * Fbm(x / 30.0, y / 30.0, s) + 0.4 * Fbm(x / 11.0, y / 11.0, s + 50, 3);
	h += BiomeLift[static_cast<std::size_t>(baseBiome(x, y))];
	h += 0.06 * relief(x, y);
	const auto land = continent(x, y);
	const auto sea = 1.0 - land;
	h -= 0.42 * sea;
	// Islands : a ragged noise over the sea, chains of them
	const auto isles = Fbm(x / 46.0, y / 46.0, s + 520, 3);
	if (isles > 0.6) {
		h += 0.55 * sea * Smooth(std::min(1.0, (isles - 0.6) / 0.18));
	}
	return h;
}

worldgen::Sea worldgen::Infinite::ocean(int x, int y) {
	const auto land = continent(x, y);
	if (land > 0.6) {
		return Sea::None;
	}
	const auto h = height(x, y);
	if (0.40 <= h && h < 0.415 && land < 0.3) {
		return Sea::Bar;
	}
	if (h >= 0.40) {
		return Sea::None;
	}
	if (h < 0.27) {
		return Sea::Deep;
	}
	if (h > 0.34 && Fbm(x / 5.0, y / 5.0, m_seed + 530, 2) > 0.66) {
		return Sea::Reef;
	}
	return Sea::Shallows;
}

// ---------------------------------------------------------------- the villages

std::optional<std::pair<worldgen::Block, worldgen::Biome>> worldgen::Infinite::site(int i, int j) {
	const auto cs = CellSeed(m_seed, i, j);
	if (H01(cs, 0, 77) >= VillageChance) {
		return std::nullopt;
	}
	constexpr auto span = (VillageCell - 2 * VillageMargin) / VillageTries;
	auto best = std::optional<std::pair<Block, Biome>>{};
	auto bestScore = 0.0;
	for (auto t = 0; t < VillageTries * VillageTries; t++) {
		const auto x = i * VillageCell + VillageMargin + t % VillageTries * span + static_cast<int>(H01(cs, 2 * t + 1, 77) * span);
		const auto y = j * VillageCell + VillageMargin + t / VillageTries * span + static_cast<int>(H01(cs, 2 * t + 2, 77) * span);
		const auto r = relief(x, y);
		if (continent(x, y) < VillageLand || r > 0.55 || landHeight(x, y) < 0.45) {
			continue;
		}
		const auto b = baseBiome(x, y);
		auto inside = 0;
		auto count = 0;
		for (auto dx = -20; dx < 21; dx += 5) {
			for (auto dy = -18; dy < 19; dy += 6) {
				inside += baseBiome(x + dx, y + dy) == b ? 1 : 0;
				count++;
			}
		}
		const auto share = static_cast<double>(inside) / static_cast<double>(count);
		if (share < VillageShare) {
			continue;
		}
		const auto score = share - r + 0.05 * H01(x, y, m_seed + 7);
		if (!best || score > bestScore) {
			best = std::make_pair(Block{ x, y }, b);
			bestScore = score;
		}
	}
	return best;
}

worldgen::Cell2 worldgen::Infinite::start() {
	if (m_start) {
		return *m_start;
	}
	const auto c = Center / VillageCell;
	auto found = std::optional<Cell2>{};
	for (const auto want : { std::optional<Biome>{ Biome::Meadow }, std::optional<Biome>{} }) {
		auto best = std::optional<std::pair<std::tuple<long long, int, int>, Cell2>>{};
		for (auto r = 0; r <= StartSearch; r++) {
			for (auto j = c - r; j <= c + r; j++) {
				for (auto i = c - r; i <= c + r; i++) {
					if (std::max(std::abs(i - c), std::abs(j - c)) != r) {
						continue;
					}
					const auto got = site(i, j);
					if (!got || (want && got->second != *want)) {
						continue;
					}
					const auto dx = static_cast<long long>(got->first.first - Center);
					const auto dy = static_cast<long long>(got->first.second - Center);
					const auto key = std::make_tuple(dx * dx + dy * dy, j, i);
					if (!best || key < best->first) {
						best = std::make_pair(key, Cell2{ i, j });
					}
				}
			}
			if (best && r > std::max(std::abs(best->second.first - c), std::abs(best->second.second - c))) {
				break;
			}
		}
		if (best) {
			found = best->second;
			break;
		}
	}
	m_start = found.value_or(Cell2{ c, c });
	return *m_start;
}

const worldgen::CellVillage* worldgen::Infinite::village(int i, int j) {
	const auto key = Cell2{ i, j };
	if (const auto found = m_villages.find(key); found != m_villages.end()) {
		return found->second.get();
	}
	auto made = std::unique_ptr<CellVillage>{};
	if (const auto got = site(i, j)) {
		const auto [at, b] = *got;
		const auto first = key == start();
		auto rng = Rng{ CellSeed(m_seed + 1000, i, j) };
		const auto& style = StyleOf(b);
		made = std::make_unique<CellVillage>(Village{ at.first, at.second, rng, style.roofs, first ? 5 : 3, first ? 8 : 6 });
		auto& v = made->v;
		v.biome = b;
		made->first = first;
		made->cell = key;
		const auto form = rng.choice(4);
		v.name = NameForm(form, style.words[rng.choice(style.words.size())]);
		made->nearBiome = DistanceTo(v.cells, VillageBiome);
		for (const auto& [c, d] : made->nearBiome) {
			made->nearBiomeFast.emplace(Key(c.first, c.second), d);
			if (d <= VillageLevel) {
				made->near.emplace(Key(c.first, c.second), d);
			}
		}
	}
	const auto* out = made.get();
	m_villages.emplace(key, std::move(made));
	return out;
}

// ---------------------------------------------------------------- the roads

const std::vector<worldgen::Cell2>& worldgen::Infinite::nearest(int i, int j) {
	static const auto none = std::vector<Cell2>{};
	if (const auto found = m_nearest.find({ i, j }); found != m_nearest.end()) {
		return found->second;
	}
	const auto* v = village(i, j);
	if (v == nullptr) {
		return none;
	}
	auto near = std::vector<std::tuple<long long, int, int>>{};
	for (auto sj = j - RoadCells; sj <= j + RoadCells; sj++) {
		for (auto si = i - RoadCells; si <= i + RoadCells; si++) {
			if (si == i && sj == j) {
				continue;
			}
			const auto* o = village(si, sj);
			if (o == nullptr) {
				continue;
			}
			const auto dx = static_cast<long long>(o->v.center.first - v->v.center.first);
			const auto dy = static_cast<long long>(o->v.center.second - v->v.center.second);
			const auto d = dx * dx + dy * dy;
			if (d <= static_cast<long long>(RoadReach) * RoadReach) {
				near.emplace_back(d, sj, si);
			}
		}
	}
	std::sort(near.begin(), near.end());
	auto out = std::vector<Cell2>{};
	for (auto k = 0u; k < near.size() && k < RoadNearest; k++) {
		out.push_back({ std::get<2>(near[k]), std::get<1>(near[k]) });
	}
	return m_nearest.emplace(Cell2{ i, j }, std::move(out)).first->second;
}

std::vector<worldgen::Cell2> worldgen::Infinite::links(int i, int j) {
	const auto& own = nearest(i, j);
	auto out = std::set<Cell2>(own.begin(), own.end());
	for (auto sj = j - RoadCells; sj <= j + RoadCells; sj++) {
		for (auto si = i - RoadCells; si <= i + RoadCells; si++) {
			if (si == i && sj == j) {
				continue;
			}
			const auto& other = nearest(si, sj);
			if (std::find(other.begin(), other.end(), Cell2{ i, j }) != other.end()) {
				out.insert({ si, sj });
			}
		}
	}
	return { out.begin(), out.end() };
}

std::array<int, 4> worldgen::Infinite::roadBox(const Village& a, const Village& b) const {
	const auto [ax, ay] = a.center;
	const auto [bx, by] = b.center;
	return { std::min(ax, bx) - RoadMargin, std::min(ay, by) - RoadMargin, std::max(ax, bx) + RoadMargin, std::max(ay, by) + RoadMargin };
}

const worldgen::Blocks* worldgen::Infinite::road(Cell2 ca, Cell2 cb) {
	if (cb < ca) {
		std::swap(ca, cb);
	}
	auto& entry = m_roads[{ ca, cb }];
	if (entry.done) {
		return entry.blocks ? &*entry.blocks : nullptr;
	}
	const auto& a = village(ca.first, ca.second)->v;
	const auto& b = village(cb.first, cb.second)->v;
	const auto box = roadBox(a, b);
	auto blocked = std::unordered_set<std::uint64_t>{};
	for (auto cj = FloorDiv(box[1], VillageCell); cj <= FloorDiv(box[3], VillageCell); cj++) {
		for (auto ci = FloorDiv(box[0], VillageCell); ci <= FloorDiv(box[2], VillageCell); ci++) {
			const auto* o = village(ci, cj);
			if (o == nullptr) {
				continue;
			}
			for (const auto* set : { &o->v.footprint, &o->v.pond }) {
				for (const auto& [x, y] : *set) {
					blocked.insert(Key(x, y));
				}
			}
		}
	}
	auto memo = std::unordered_map<std::uint64_t, double>{};
	const auto cost = [&](int x, int y) {
		const auto key = Key(x, y);
		if (const auto found = memo.find(key); found != memo.end()) {
			return found->second;
		}
		auto c = -1.0;
		if (box[0] <= x && x <= box[2] && box[1] <= y && y <= box[3] && blocked.count(key) == 0 && height(x, y) >= 0.41) {
			const auto* o = villageOf(x, y);
			const auto near = o != nullptr && o->near.count(key) > 0;
			const auto edge = std::min({ x - box[0], box[2] - x, y - box[1], box[3] - y });
			c = 1.0 + 10.0 * relief(x, y) + (near ? 0.0 : 0.5) + 3.0 * Fbm(x / 6.0, y / 6.0, m_seed + 500, 2) + 0.5 * std::max(0, RoadEdge - edge);
		}
		memo.emplace(key, c);
		return c;
	};
	auto path = AStar(cost, a.center, b.center);
	if (path) {
		auto blocks = Blocks{};
		for (auto n = 0u; n < path->size(); n++) {
			const auto [x, y] = (*path)[n];
			blocks.insert({ x, y });
			const auto [nx, ny] = (*path)[std::min<std::size_t>(n + 1, path->size() - 1)];
			const auto side = nx == x ? Block{ x + 1, y } : Block{ x, y + 1 };
			if (cost(side.first, side.second) >= 0.0) {
				blocks.insert(side);
			}
		}
		for (auto it = blocks.begin(); it != blocks.end();) {
			it = blocked.count(Key(it->first, it->second)) > 0 ? blocks.erase(it) : std::next(it);
		}
		// (the map may have grown : the entry again)
		auto& done = m_roads[{ ca, cb }];
		done.blocks = std::move(blocks);
		done.path = std::move(path);
	}
	auto& done = m_roads[{ ca, cb }];
	done.done = true;
	return done.blocks ? &*done.blocks : nullptr;
}

const std::vector<worldgen::Block>* worldgen::Infinite::roadPath(Cell2 ca, Cell2 cb) {
	road(ca, cb);
	const auto& entry = m_roads[{ std::min(ca, cb), std::max(ca, cb) }];
	return entry.path ? &*entry.path : nullptr;
}

std::vector<std::pair<worldgen::Cell2, worldgen::Cell2>> worldgen::Infinite::roads(int ci0, int cj0, int ci1, int cj1) {
	auto out = std::set<std::pair<Cell2, Cell2>>{};
	for (auto cj = cj0; cj <= cj1; cj++) {
		for (auto ci = ci0; ci <= ci1; ci++) {
			for (const auto& o : links(ci, cj)) {
				const auto c = Cell2{ ci, cj };
				out.insert({ std::min(c, o), std::max(c, o) });
			}
		}
	}
	return { out.begin(), out.end() };
}

const std::unordered_set<std::uint64_t>& worldgen::Infinite::roadsOfCell(int i, int j) {
	if (const auto found = m_cellRoads.find({ i, j }); found != m_cellRoads.end()) {
		return found->second;
	}
	const auto x0 = i * VillageCell - 3;
	const auto y0 = j * VillageCell - 3;
	const auto x1 = x0 + VillageCell + 6;
	const auto y1 = y0 + VillageCell + 6;
	auto out = std::unordered_set<std::uint64_t>{};
	for (const auto& [ca, cb] : roads(i - RoadCells, j - RoadCells, i + RoadCells, j + RoadCells)) {
		if (std::max(std::abs(ca.first - i), std::abs(ca.second - j)) > RoadCells || std::max(std::abs(cb.first - i), std::abs(cb.second - j)) > RoadCells) {
			continue;
		}
		const auto box = roadBox(village(ca.first, ca.second)->v, village(cb.first, cb.second)->v);
		if (box[2] < x0 || box[0] > x1 || box[3] < y0 || box[1] > y1) {
			continue;
		}
		if (const auto* blocks = road(ca, cb)) {
			for (const auto& [x, y] : *blocks) {
				out.insert(Key(x, y));
			}
		}
	}
	return m_cellRoads.emplace(Cell2{ i, j }, std::move(out)).first->second;
}

bool worldgen::Infinite::onRoad(int x, int y) {
	return roadsOfCell(FloorDiv(x, VillageCell), FloorDiv(y, VillageCell)).count(Key(x, y)) > 0;
}

bool worldgen::Infinite::nearRoad(int x, int y, int limit) {
	const auto& blocks = roadsOfCell(FloorDiv(x, VillageCell), FloorDiv(y, VillageCell));
	if (blocks.empty()) {
		return false;
	}
	for (auto dy = -limit; dy <= limit; dy++) {
		for (auto dx = -limit + std::abs(dy); dx <= limit - std::abs(dy); dx++) {
			if (blocks.count(Key(x + dx, y + dy)) > 0) {
				return true;
			}
		}
	}
	return false;
}

// ---------------------------------------------------------------- the terrain

worldgen::Cell worldgen::Infinite::classify(int x, int y) {
	const auto* v = villageOf(x, y);
	if (v != nullptr) {
		const auto b = Block{ x, y };
		if (v->v.road.count(b) > 0 || v->v.pondRing.count(b) > 0) {
			return { Ground::Pave, 0 };
		}
		if (v->v.pond.count(b) > 0) {
			return { Ground::Water, 0 };
		}
		if (v->v.yard.count(b) > 0) {
			return { Ground::Sand, 0 };
		}
	}
	if (onRoad(x, y)) {
		return { Ground::Path, 0 };
	}
	const auto h = height(x, y);
	const auto sea = ocean(x, y);
	if (sea == Sea::Bar) {
		return { Ground::Sand, 0 };
	}
	if (sea == Sea::Reef) {
		return { Ground::Reef, 0 };
	}
	if (h < 0.40) {
		return { Ground::Water, 0 };
	}
	if (h < 0.425) {
		return { Ground::Sand, 0 };
	}
	if (v != nullptr) {
		const auto found = v->near.find(Key(x, y));
		if (found != v->near.end() && found->second <= 6) {
			return { Ground::Grass, 0 };
		}
	}
	if (nearRoad(x, y)) {
		return { Ground::Grass, 0 };
	}
	const auto a = relief(x, y) * continent(x, y);
	const auto plateau = Fbm(x / 15.0, y / 15.0, m_seed + 60, 3) > 0.6 && h > 0.47;
	const auto m = (h - 0.43) * 14 * a + 1.2 * a * (Fbm(x / 7.0, y / 7.0, m_seed + 330, 2) - 0.5);
	return { Ground::Grass, std::max(plateau ? 1 : 0, std::min(MaxLevel, static_cast<int>(std::max(0.0, m)))) };
}

worldgen::Area worldgen::Infinite::area(int x0, int y0, int x1, int y1) {
	const auto win = Window{ *this, x0 - Apron, y0 - Apron, x1 + Apron, y1 + Apron };
	auto out = Area{};
	out.x0 = x0;
	out.y0 = y0;
	out.width = x1 - x0;
	out.height = y1 - y0;
	out.terrain.reserve(static_cast<std::size_t>(out.width) * static_cast<std::size_t>(out.height));
	for (auto j = 0; j < out.height; j++) {
		for (auto i = 0; i < out.width; i++) {
			out.terrain.push_back(win.t(Apron + i, Apron + j));
		}
	}
	for (auto j = 0; j < out.height; j++) {
		for (auto i = 0; i < out.width; i++) {
			if (win.stairs()[static_cast<std::size_t>(Apron + j) * static_cast<std::size_t>(win.width()) + static_cast<std::size_t>(Apron + i)]) {
				out.stairs.push_back({ x0 + i, y0 + j });
			}
		}
	}
	for (const auto& patch : win.unreached()) {
		if (std::any_of(patch.begin(), patch.end(), [&](const Block& c) {
			return Apron <= c.first && c.first < Apron + out.width && Apron <= c.second && c.second < Apron + out.height;
		})) {
			out.unreached++;
		}
	}
	return out;
}

std::size_t worldgen::Infinite::cached() const {
	auto total = m_climate.size() + m_base.size() + m_nearest.size();
	for (const auto& [cell, blocks] : m_cellRoads) {
		total += blocks.size();
	}
	for (const auto& [key, road] : m_roads) {
		total += road.blocks ? road.blocks->size() * 2 : 1;
	}
	for (const auto& [cell, v] : m_villages) {
		total += v ? v->nearBiome.size() * 3 : 1;
	}
	return total;
}

void worldgen::Infinite::forget() {
	m_climate.clear();
	m_base.clear();
	m_villages.clear();
	m_roads.clear();
	m_cellRoads.clear();
	m_nearest.clear();
}
