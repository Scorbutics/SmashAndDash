#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <string>
#include "Climate.h"
#include "Noise.h"

const char* worldgen::BiomeName(Biome biome) {
	switch (biome) {
	case Biome::Autumn: return "autumn";
	case Biome::Snow: return "snow";
	case Biome::Desert: return "desert";
	default: return "meadow";
	}
}

worldgen::Biome worldgen::ClimateBiome(double temperature, double humidity) {
	if (temperature < 0.28) {
		return Biome::Snow;
	}
	if (temperature > 0.72) {
		return Biome::Desert;
	}
	return humidity > 0.55 ? Biome::Autumn : Biome::Meadow;
}

namespace {
	// The 4 neighbours, in the order of the Python generator (D8[:4] : N E S W)
	constexpr std::array<std::array<int, 2>, 4> Neighbours{ { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } } };

	// The biomes sorted by their names (a tie of counts goes to the first, as max(sorted(...)) in Python)
	std::vector<worldgen::Biome> ByName() {
		auto biomes = std::vector<worldgen::Biome>{};
		for (auto b = 0; b < worldgen::BiomeCount; b++) {
			biomes.push_back(static_cast<worldgen::Biome>(b));
		}
		std::sort(biomes.begin(), biomes.end(), [](worldgen::Biome a, worldgen::Biome b) {
			return std::string{ worldgen::BiomeName(a) } < std::string{ worldgen::BiomeName(b) };
		});
		return biomes;
	}

	worldgen::Biome MostCounted(const std::array<int, worldgen::BiomeCount>& counts) {
		auto best = worldgen::Biome::Meadow;
		auto bestCount = -1;
		for (const auto biome : ByName()) {
			const auto count = counts[static_cast<std::size_t>(biome)];
			if (count > 0 && count > bestCount) {
				best = biome;
				bestCount = count;
			}
		}
		return best;
	}
}

worldgen::Climate::Climate(std::int64_t seed, int chunksX, int chunksY) :
	m_seed(seed),
	m_chunksX(chunksX),
	m_chunksY(chunksY),
	m_width(chunksX * Chunk),
	m_height(chunksY * Chunk),
	m_label(static_cast<std::size_t>(m_width * m_height)),
	m_relief(static_cast<std::size_t>(m_width * m_height)),
	m_chunks(static_cast<std::size_t>(chunksX * chunksY)) {

	// Off the lattice of the noise
	const auto ox = H01(seed, 1, 301) * 500;
	const auto oy = H01(seed, 2, 301) * 500;
	// A field sampled every 4 blocks, sorted : the rank of a value over the world
	auto field = [&](std::int64_t fieldSeed) {
		auto values = std::vector<double>{};
		for (auto y = 0; y < m_height; y += 4) {
			for (auto x = 0; x < m_width; x += 4) {
				values.push_back(Fbm((x + ox) / ClimateScale, (y + oy) / ClimateScale, fieldSeed, 3));
			}
		}
		std::sort(values.begin(), values.end());
		return values;
	};
	auto rank = [](const std::vector<double>& sorted, double value) {
		const auto position = std::lower_bound(sorted.begin(), sorted.end(), value) - sorted.begin();
		return static_cast<double>(position) / static_cast<double>(sorted.size());
	};
	const auto temperatures = field(seed + 300);
	const auto humidities = field(seed + 310);
	const auto reliefs = field(seed + 320);

	// The relief : 0 flat .. 1 mountains (no warp : the mountains cross the borders of the biomes)
	for (auto y = 0; y < m_height; y++) {
		for (auto x = 0; x < m_width; x++) {
			const auto r = rank(reliefs, Fbm((x + ox) / ClimateScale, (y + oy) / ClimateScale, seed + 320, 3));
			m_relief[index(x, y)] = Smooth(std::max(0.0, std::min(1.0, (r - ReliefFrom) / (ReliefFull - ReliefFrom))));
		}
	}
	// The biomes : the climate at a warped position (the borders ragged at two scales)
	for (auto y = 0; y < m_height; y++) {
		for (auto x = 0; x < m_width; x++) {
			const auto wx = x + 12 * (Fbm(x / 30.0, y / 30.0, seed + 404, 2) - 0.5) / 0.25
				+ 4 * (Fbm(x / 9.0, y / 9.0, seed + 400, 3) - 0.5) / 0.25;
			const auto wy = y + 12 * (Fbm(x / 30.0, y / 30.0, seed + 405, 2) - 0.5) / 0.25
				+ 4 * (Fbm(x / 9.0, y / 9.0, seed + 401, 3) - 0.5) / 0.25;
			const auto t = Fbm((wx + ox) / ClimateScale, (wy + oy) / ClimateScale, seed + 300, 3);
			const auto h = Fbm((wx + ox) / ClimateScale, (wy + oy) / ClimateScale, seed + 310, 3);
			m_label[index(x, y)] = ClimateBiome(rank(temperatures, t), rank(humidities, h));
		}
	}
	mergeSmallBiomes();
	// The biome of a chunk
	for (auto cy = 0; cy < m_chunksY; cy++) {
		for (auto cx = 0; cx < m_chunksX; cx++) {
			auto counts = std::array<int, BiomeCount>{};
			for (auto y = cy * Chunk; y < cy * Chunk + Chunk; y++) {
				for (auto x = cx * Chunk; x < cx * Chunk + Chunk; x++) {
					counts[static_cast<std::size_t>(label(x, y))]++;
				}
			}
			m_chunks[static_cast<std::size_t>(cy * m_chunksX + cx)] = MostCounted(counts);
		}
	}
}

void worldgen::Climate::mergeSmallBiomes() {
	// A region of a biome smaller than BiomeMin blocks takes the biome most of its neighbours have (3 passes at most,
	// the regions in the order of the blocks, as the Python generator)
	for (auto pass = 0; pass < 3; pass++) {
		auto seen = std::vector<bool>(m_label.size(), false);
		auto changed = false;
		for (auto y0 = 0; y0 < m_height; y0++) {
			for (auto x0 = 0; x0 < m_width; x0++) {
				if (seen[index(x0, y0)]) {
					continue;
				}
				const auto biome = label(x0, y0);
				auto region = std::vector<std::array<int, 2>>{ { x0, y0 } };
				auto front = region;
				auto around = std::array<int, BiomeCount>{};
				seen[index(x0, y0)] = true;
				while (!front.empty()) {
					const auto [x, y] = front.back();
					front.pop_back();
					for (const auto& [dx, dy] : Neighbours) {
						const auto nx = x + dx;
						const auto ny = y + dy;
						if (nx < 0 || nx >= m_width || ny < 0 || ny >= m_height) {
							continue;
						}
						const auto neighbour = label(nx, ny);
						if (neighbour != biome) {
							around[static_cast<std::size_t>(neighbour)]++;
						} else if (!seen[index(nx, ny)]) {
							seen[index(nx, ny)] = true;
							region.push_back({ nx, ny });
							front.push_back({ nx, ny });
						}
					}
				}
				const auto any = std::any_of(around.begin(), around.end(), [](int count) { return count > 0; });
				if (static_cast<int>(region.size()) < BiomeMin && any) {
					const auto to = MostCounted(around);
					for (const auto& [x, y] : region) {
						m_label[index(x, y)] = to;
					}
					changed = true;
				}
			}
		}
		if (!changed) {
			break;
		}
	}
}

double worldgen::Climate::rawHeight(int x, int y) const {
	auto h = 0.6 * Fbm(x / 30.0, y / 30.0, m_seed) + 0.4 * Fbm(x / 11.0, y / 11.0, m_seed + 50, 3);
	// The mountains raise the land
	h += 0.06 * relief(x, y);
	// An island : the sea all around the world
	const auto border = std::min({ x, y, m_width - 1 - x, m_height - 1 - y });
	h -= 0.45 * Smooth(std::max(0.0, 1 - border / 10.0));
	// No village yet : the levelled ground around them comes later
	const auto w = 0.0;
	return h * (1 - w) + 0.52 * w;
}
