#include <algorithm>
#include <cmath>
#include <deque>
#include <sstream>
#include <tuple>
#include "Villages.h"
#include "Noise.h"

double worldgen::Rng::random() {
	m_n++;
	return H01(m_n, m_seed, 4242);
}

int worldgen::Rng::randint(int a, int b) {
	return a + static_cast<int>(random() * (b - a + 1));
}

std::size_t worldgen::Rng::choice(std::size_t count) {
	return std::min(count - 1, static_cast<std::size_t>(random() * static_cast<double>(count)));
}

namespace {
	// math.hypot of whole numbers : correctly rounded, as the square root of their exact sum
	double Hypot(int dx, int dy) {
		return std::sqrt(static_cast<double>(dx * dx + dy * dy));
	}

	bool Intersects(const worldgen::Blocks& a, const worldgen::Blocks& b) {
		const auto& small = a.size() < b.size() ? a : b;
		const auto& large = a.size() < b.size() ? b : a;
		return std::any_of(small.begin(), small.end(), [&large](const worldgen::Block& c) { return large.count(c) > 0; });
	}

	constexpr std::array<std::array<int, 2>, 8> D8{ { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 }, { 1, -1 }, { 1, 1 }, { -1, 1 }, { -1, -1 } } };

}

std::string worldgen::NameForm(std::size_t form, const std::string& word) {
	switch (form) {
	case 0: return "Bourg-" + word;
	case 1: return word + "ville";
	case 2: return "Village " + word;
	default: return "Hameau " + word;
	}
}

worldgen::Village::Village(int cx, int cy, Rng& rng, const std::vector<std::string>& roofs, int housesMin, int housesMax) :
	center{ cx, cy } {
	// The plaza : a rough disc
	for (auto y = cy - 3; y < cy + 4; y++) {
		for (auto x = cx - 3; x < cx + 4; x++) {
			const auto d = Hypot(x - cx, y - cy);
			if (d < 2.2 + rng.random() * 1.3) {
				road.insert({ x, y });
			}
		}
	}
	// Roads : a few walks from the plaza, 2 blocks wide, each segment straight then a turn, sometimes a branch
	struct Walk { int x, y, dx, dy, depth; };
	const std::array<std::array<int, 2>, 4> dirs{ { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } } };
	auto queue = std::deque<Walk>{};
	for (const auto& [dx, dy] : dirs) {
		if (rng.random() < 0.8) {
			queue.push_back(Walk{ cx, cy, dx, dy, 0 });
		}
	}
	if (queue.empty()) {
		queue.push_back(Walk{ cx, cy, 1, 0, 0 });
	}
	while (!queue.empty()) {
		auto walk = queue.front();
		queue.pop_front();
		auto x = walk.x, y = walk.y, dx = walk.dx, dy = walk.dy;
		const auto segments = walk.depth == 0 ? rng.randint(1, 3) : rng.randint(1, 2);
		for (auto s = 0; s < segments; s++) {
			const auto steps = rng.randint(4, 9);
			for (auto k = 0; k < steps; k++) {
				x += dx;
				y += dy;
				if (std::abs(x - cx) > 17 || std::abs(y - cy) > 15) {
					break;
				}
				road.insert({ x, y });
				road.insert({ x + std::abs(dy), y + std::abs(dx) });
				if (walk.depth < 2 && rng.random() < 0.07) {
					const auto side = rng.choice(2);
					queue.push_back(side == 0 ? Walk{ x, y, dy, dx, walk.depth + 1 } : Walk{ x, y, -dy, -dx, walk.depth + 1 });
				}
			}
			if (rng.random() < 0.5) {
				const auto turn = rng.choice(2);
				const auto ndx = turn == 0 ? dy : -dy;
				const auto ndy = turn == 0 ? dx : -dx;
				dx = ndx;
				dy = ndy;
			}
		}
	}
	// Buildings : along the roads, set back from them, a short path to their door
	struct Kind { std::string kind, roof; int w, h, door; };
	auto kinds = std::vector<Kind>{ Kind{ "center", "", 5, 5, 2 } };
	const auto houses = rng.randint(housesMin, housesMax);
	for (auto i = 0; i < houses; i++) {
		kinds.push_back(Kind{ "house", roofs[rng.choice(roofs.size())], 4, 4, 2 });
	}
	struct Spot { double key; std::string side; int rx, ry; };
	auto spots = std::vector<Spot>{};
	for (const auto& [rx, ry] : road) {
		if (road.count({ rx, ry - 1 }) == 0) {
			spots.push_back(Spot{ 0, "north", rx, ry });
		}
		if (road.count({ rx - 1, ry }) == 0 && road.count({ rx, ry - 1 }) > 0) {
			spots.push_back(Spot{ 0, "west", rx, ry });
		}
		if (road.count({ rx + 1, ry }) == 0 && road.count({ rx, ry - 1 }) > 0) {
			spots.push_back(Spot{ 0, "east", rx, ry });
		}
	}
	for (auto& spot : spots) {
		spot.key = H01(static_cast<std::int64_t>(spot.rx) * 3 + static_cast<std::int64_t>(spot.side.size()), spot.ry, rng.seed() + 17);
	}
	std::sort(spots.begin(), spots.end(), [](const Spot& a, const Spot& b) {
		return std::tie(a.key, a.side, a.rx, a.ry) < std::tie(b.key, b.side, b.rx, b.ry);
	});
	for (const auto& kind : kinds) {
		for (const auto& spot : spots) {
			const auto gap = rng.randint(0, 2);
			auto x0 = 0, y0 = 0;
			auto path = Blocks{};
			if (spot.side == "north") {
				x0 = spot.rx - kind.door;
				y0 = spot.ry - gap - kind.h;
				for (auto py = spot.ry - gap; py < spot.ry; py++) {
					path.insert({ spot.rx, py });
				}
			} else if (spot.side == "west") {
				x0 = spot.rx - 1 - gap - kind.w;
				y0 = spot.ry - kind.h;
				for (auto px = x0 + kind.door; px < spot.rx; px++) {
					path.insert({ px, spot.ry });
				}
			} else {
				x0 = spot.rx + 2 + gap;
				y0 = spot.ry - kind.h;
				for (auto px = spot.rx + 1; px < x0 + kind.door + 1; px++) {
					path.insert({ px, spot.ry });
				}
			}
			auto body = Blocks{};
			for (auto bx = x0 - 1; bx < x0 + kind.w + 1; bx++) {
				for (auto by = y0; by < y0 + kind.h; by++) {
					body.insert({ bx, by });
				}
			}
			if (Intersects(body, road) || Intersects(body, footprint) || Intersects(path, footprint) || Intersects(path, body)) {
				continue;
			}
			if (std::abs(x0 - cx) > 19 || std::abs(y0 - cy) > 17) {
				continue;
			}
			buildings.push_back(Building{ kind.kind, kind.roof, x0, y0, kind.w, kind.h, kind.door, kind.h - 1 });
			for (auto bx = x0 - 1; bx < x0 + kind.w + 1; bx++) {
				for (auto by = y0 - 1; by < y0 + kind.h; by++) {
					footprint.insert({ bx, by });
				}
			}
			road.insert(path.begin(), path.end());
			break;
		}
	}
	// Gardens : flowers along some buildings
	for (const auto& b : buildings) {
		if (b.kind == "house" && rng.random() < 0.6) {
			const auto side = rng.random() < 0.5 ? b.x0 - 1 : b.x0 + b.w;
			for (auto y = b.y0 + 1; y < b.y0 + b.h; y++) {
				if (road.count({ side, y }) == 0) {
					flowers.insert({ side, y });
				}
			}
		}
	}
	// Yards : sand in front of the buildings
	for (const auto& b : buildings) {
		for (auto x = b.x0; x < b.x0 + b.w; x++) {
			if (road.count({ x, b.y0 + b.h }) == 0 && footprint.count({ x, b.y0 + b.h }) == 0) {
				yard.insert({ x, b.y0 + b.h });
			}
		}
	}
	// A pond near the plaza (half of the villages) : 4 x 3 water in a ring of paving
	if (rng.random() < 0.6) {
		auto taken = road;
		taken.insert(footprint.begin(), footprint.end());
		taken.insert(flowers.begin(), flowers.end());
		taken.insert(yard.begin(), yard.end());
		for (auto attempt = 0; attempt < 40; attempt++) {
			const auto px = cx + rng.randint(-12, 8);
			const auto py = cy + rng.randint(-10, 7);
			auto ring = Blocks{};
			for (auto x = px - 1; x < px + 5; x++) {
				for (auto y = py - 1; y < py + 4; y++) {
					ring.insert({ x, y });
				}
			}
			auto blocked = Intersects(ring, taken);
			for (auto it = ring.begin(); !blocked && it != ring.end(); ++it) {
				for (const auto& [dx, dy] : D8) {
					if (taken.count({ it->first + dx, it->second + dy }) > 0) {
						blocked = true;
						break;
					}
				}
			}
			if (blocked) {
				continue;
			}
			for (auto x = px; x < px + 4; x++) {
				for (auto y = py; y < py + 3; y++) {
					pond.insert({ x, y });
				}
			}
			for (const auto& c : ring) {
				if (pond.count(c) == 0) {
					pondRing.insert(c);
				}
			}
			break;
		}
	}
	for (const auto* set : { &road, &footprint, &flowers, &yard, &pond, &pondRing }) {
		cells.insert(set->begin(), set->end());
	}
}

const worldgen::VillageStyle& worldgen::StyleOf(Biome biome) {
	static const auto styles = std::map<Biome, VillageStyle>{
		{ Biome::Meadow, VillageStyle{ { "roofr", "roofb", "roofg", "roofo" }, { "Trèfle", "Verdure", "Prairie", "Bourgeon", "Coquelicot" }, "Un village paisible au milieu des prés." } },
		{ Biome::Autumn, VillageStyle{ { "roofr", "roofo", "roofg" }, { "Châtaigne", "Noisette", "Feuillage", "Brume", "Rousse" }, "Les feuilles y tombent toute l'année." } },
		{ Biome::Snow, VillageStyle{ { "roofb", "roofr", "roofg" }, { "Givre", "Frimas", "Flocon", "Glace", "Neige" }, "L'hiver ne finit jamais ici." } },
		{ Biome::Desert, VillageStyle{ { "roofo", "roofr" }, { "Dune", "Sable", "Ocre", "Mirage", "Soleil" }, "La chaleur ne fait pas peur à ses habitants." } },
	};
	return styles.at(biome);
}

std::string worldgen::Village::buildingsRepr() const {
	auto out = std::ostringstream{};
	out << "[";
	for (auto i = 0u; i < buildings.size(); i++) {
		const auto& b = buildings[i];
		out << (i ? ", " : "") << "('" << b.kind << "', " << (b.roof.empty() ? std::string{ "None" } : "'" + b.roof + "'") << ", "
			<< b.x0 << ", " << b.y0 << ", " << b.w << ", " << b.h << ", " << b.door << ", " << b.doorRow << ")";
	}
	out << "]";
	return out.str();
}

std::map<worldgen::Block, int> worldgen::DistanceTo(const Blocks& cells, int limit) {
	auto dist = std::map<Block, int>{};
	auto front = std::vector<Block>{};
	for (const auto& c : cells) {
		dist[c] = 0;
		front.push_back(c);
	}
	for (auto d = 1; d <= limit; d++) {
		auto next = std::vector<Block>{};
		for (const auto& [x, y] : front) {
			for (auto k = 0; k < 4; k++) {
				const auto c = Block{ x + D8[static_cast<std::size_t>(k)][0], y + D8[static_cast<std::size_t>(k)][1] };
				if (dist.count(c) == 0) {
					dist[c] = d;
					next.push_back(c);
				}
			}
		}
		front = std::move(next);
	}
	return dist;
}

worldgen::WorldPlan::WorldPlan(std::int64_t seed, int chunksX, int chunksY) :
	m_seed(seed),
	m_climate(std::in_place, seed, chunksX, chunksY),
	m_width(chunksX * Chunk),
	m_height(chunksY * Chunk) {
	m_town = villageSite();
	planVillages();
	auto all = Blocks{};
	for (const auto& v : m_villages) {
		all.insert(v.cells.begin(), v.cells.end());
	}
	m_nearVillage = DistanceTo(all, 8);
	biomeField();
}

worldgen::Block worldgen::WorldPlan::villageSite() const {
	const auto W = m_climate->width();
	const auto H = m_climate->height();
	const auto cx = W / 2;
	const auto cy = H / 2;
	auto best = Block{ cx / Chunk * Chunk + Chunk / 2, cy / Chunk * Chunk + Chunk / 2 };
	auto hasBest = false;
	auto bestScore = 0.0;
	for (auto y = 25; y < H - 25; y += 2) {
		for (auto x = 25; x < W - 25; x += 2) {
			if (std::abs(x % Chunk - Chunk / 2) > 6 || std::abs(y % Chunk - Chunk / 2) > 6 || m_climate->rawHeight(x, y) < 0.45) {
				continue;
			}
			auto meadow = 0;
			auto count = 0;
			for (auto dx = -20; dx < 21; dx += 4) {
				for (auto dy = -18; dy < 19; dy += 4) {
					meadow += m_climate->label(x + dx, y + dy) == Biome::Meadow ? 1 : 0;
					count++;
				}
			}
			const auto score = static_cast<double>(meadow) / static_cast<double>(count) - Hypot(x - cx, y - cy) / (W + H) - 2 * m_climate->relief(x, y);
			if (!hasBest || score > bestScore) {
				best = Block{ x, y };
				bestScore = score;
				hasBest = true;
			}
		}
	}
	return best;
}

worldgen::Biome worldgen::WorldPlan::chunkBiome(int cx, int cy) const {
	if (m_climate) {
		return m_climate->chunkBiome(cx, cy);
	}
	const auto found = m_chunkBiomes.find({ cx, cy });
	return found == m_chunkBiomes.end() ? Biome::Meadow : found->second;
}

void worldgen::WorldPlan::planVillages() {
	const auto W = m_climate->width();
	const auto H = m_climate->height();
	auto sites = std::vector<std::pair<Block, Biome>>{ { m_town, Biome::Meadow } };
	for (const auto biome : { Biome::Desert, Biome::Snow, Biome::Autumn, Biome::Meadow, Biome::Desert, Biome::Snow, Biome::Autumn }) {
		auto best = Block{};
		auto hasBest = false;
		auto bestScore = 0.0;
		for (auto cy = 0; cy < H / Chunk; cy++) {
			for (auto cx = 0; cx < W / Chunk; cx++) {
				for (auto y = cy * Chunk + Chunk / 2 - 6; y < cy * Chunk + Chunk / 2 + 7; y += 2) {
					for (auto x = cx * Chunk + Chunk / 2 - 6; x < cx * Chunk + Chunk / 2 + 7; x += 2) {
						if (!(25 <= x && x < W - 25 && 25 <= y && y < H - 25)) {
							continue;
						}
						if (m_climate->relief(x, y) > 0.55 || m_climate->rawHeight(x, y) < 0.45) {
							continue;
						}
						if (std::any_of(sites.begin(), sites.end(), [x, y](const auto& site) { return Hypot(x - site.first.first, y - site.first.second) < VillageSpacing; })) {
							continue;
						}
						auto inside = 0;
						auto count = 0;
						for (auto dx = -20; dx < 21; dx += 5) {
							for (auto dy = -18; dy < 19; dy += 6) {
								inside += m_climate->label(x + dx, y + dy) == biome ? 1 : 0;
								count++;
							}
						}
						const auto share = static_cast<double>(inside) / static_cast<double>(count);
						if (share < 0.7) {
							continue;
						}
						const auto score = share - m_climate->relief(x, y) + 0.05 * H01(x, y, m_seed + 7);
						if (!hasBest || score > bestScore) {
							best = Block{ x, y };
							bestScore = score;
							hasBest = true;
						}
					}
				}
			}
		}
		if (hasBest) {
			sites.emplace_back(best, biome);
		}
	}
	auto used = std::set<std::string>{};
	for (auto i = 0u; i < sites.size(); i++) {
		const auto& [site, biome] = sites[i];
		auto rng = Rng{ m_seed + 1000 * static_cast<std::int64_t>(i) };
		const auto& style = StyleOf(biome);
		auto village = Village{ site.first, site.second, rng, style.roofs, i == 0 ? 5 : 3, i == 0 ? 8 : 6 };
		village.biome = biome;
		auto words = std::vector<std::string>{};
		for (const auto& word : style.words) {
			if (used.count(word) == 0) {
				words.push_back(word);
			}
		}
		if (words.empty()) {
			words = style.words;
		}
		const auto word = words[rng.choice(words.size())];
		used.insert(word);
		village.name = NameForm(rng.choice(4), word);
		m_villages.push_back(std::move(village));
	}
	for (const auto& v : m_villages) {
		for (const auto& [c, d] : DistanceTo(v.cells, 12)) {
			m_villageBiome[c] = v.biome;
		}
	}
}

void worldgen::WorldPlan::biomeField() {
	const auto W = m_climate->width();
	const auto H = m_climate->height();
	auto label = std::vector<Biome>(static_cast<std::size_t>(W * H));
	for (auto y = 0; y < H; y++) {
		for (auto x = 0; x < W; x++) {
			const auto found = m_villageBiome.find({ x, y });
			label[index(x, y)] = found != m_villageBiome.end() ? found->second : m_climate->label(x, y);
		}
	}
	const auto r = 5;
	m_weights.assign(static_cast<std::size_t>(W * H * BiomeCount), 0.0);
	for (auto b = 0; b < BiomeCount; b++) {
		auto grid = std::vector<double>(static_cast<std::size_t>(W * H));
		for (auto i = 0u; i < grid.size(); i++) {
			grid[i] = label[i] == static_cast<Biome>(b) ? 1.0 : 0.0;
		}
		// Two box blurs : a smooth ramp (the same sums in the same order as the Python generator)
		for (auto pass = 0; pass < 2; pass++) {
			auto rows = std::vector<double>(grid.size());
			for (auto y = 0; y < H; y++) {
				auto pre = std::vector<double>{ 0.0 };
				for (auto x = 0; x < W; x++) {
					pre.push_back(pre.back() + grid[index(x, y)]);
				}
				for (auto x = 0; x < W; x++) {
					const auto lo = std::max(0, x - r);
					const auto hi = std::min(W, x + r + 1);
					rows[index(x, y)] = (pre[static_cast<std::size_t>(hi)] - pre[static_cast<std::size_t>(lo)]) / (hi - lo);
				}
			}
			for (auto x = 0; x < W; x++) {
				auto pre = std::vector<double>{ 0.0 };
				for (auto y = 0; y < H; y++) {
					pre.push_back(pre.back() + rows[index(x, y)]);
				}
				for (auto y = 0; y < H; y++) {
					const auto lo = std::max(0, y - r);
					const auto hi = std::min(H, y + r + 1);
					grid[index(x, y)] = (pre[static_cast<std::size_t>(hi)] - pre[static_cast<std::size_t>(lo)]) / (hi - lo);
				}
			}
		}
		for (auto i = 0u; i < grid.size(); i++) {
			if (grid[i] > 0.001) {
				m_weights[i * BiomeCount + static_cast<std::size_t>(b)] = grid[i];
			}
		}
	}
	// The corners : the mean of their 4 blocks, in MixSteps (the largest remainders round up)
	m_corners.assign(static_cast<std::size_t>((W + 1) * (H + 1)), {});
	for (auto j = 0; j < H + 1; j++) {
		for (auto i = 0; i < W + 1; i++) {
			auto acc = std::array<double, BiomeCount>{};
			for (const auto dx : { -1, 0 }) {
				for (const auto dy : { -1, 0 }) {
					const auto bx = std::min(W - 1, std::max(0, i + dx));
					const auto by = std::min(H - 1, std::max(0, j + dy));
					for (auto k = 0; k < BiomeCount; k++) {
						acc[static_cast<std::size_t>(k)] += m_weights[index(bx, by) * BiomeCount + static_cast<std::size_t>(k)];
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
			auto& c = m_corners[static_cast<std::size_t>(j) * static_cast<std::size_t>(W + 1) + static_cast<std::size_t>(i)];
			for (auto k = 0; k < BiomeCount; k++) {
				c[static_cast<std::size_t>(k)] = static_cast<std::uint8_t>(q[static_cast<std::size_t>(k)]);
			}
		}
	}
}
