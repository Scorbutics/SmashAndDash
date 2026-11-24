#pragma once
#include <cstdint>
#include <vector>

// The climate of a generated world (the C++ port of tools/worldgen : World.climate, World.height) : the biome of each
// block from a temperature and a humidity, as in Minecraft, its relief (the mountains), its height before the villages.
namespace worldgen {
	// The biomes, in the order of tools/tilegen BIOMES
	enum class Biome : std::uint8_t { Meadow = 0, Autumn = 1, Snow = 2, Desert = 3 };
	constexpr int BiomeCount = 4;
	const char* BiomeName(Biome biome);

	constexpr int Chunk = 40;
	constexpr double ClimateScale = 150.0;
	constexpr int BiomeMin = 500;
	constexpr double ReliefFrom = 0.55;
	constexpr double ReliefFull = 0.8;

	// The table of the climate (temperature, humidity : their ranks over the world, 0 .. 1)
	Biome ClimateBiome(double temperature, double humidity);

	class Climate {
	public:
		Climate(std::int64_t seed, int chunksX, int chunksY);

		std::int64_t seed() const { return m_seed; }
		int width() const { return m_width; }
		int height() const { return m_height; }
		// The biome of a block (its climate, the small regions merged), its relief (0 flat .. 1 mountains)
		Biome label(int x, int y) const { return m_label[index(x, y)]; }
		double relief(int x, int y) const { return m_relief[index(x, y)]; }
		// The biome of a chunk : the one most of its blocks have
		Biome chunkBiome(int cx, int cy) const { return m_chunks[static_cast<std::size_t>(cy * m_chunksX + cx)]; }
		// The height of a block before the villages and the biome weights (the noise, the island, the mountains)
		double rawHeight(int x, int y) const;

	private:
		std::size_t index(int x, int y) const { return static_cast<std::size_t>(y) * static_cast<std::size_t>(m_width) + static_cast<std::size_t>(x); }
		void mergeSmallBiomes();

		std::int64_t m_seed;
		int m_chunksX;
		int m_chunksY;
		int m_width;
		int m_height;
		std::vector<Biome> m_label;
		std::vector<double> m_relief;
		std::vector<Biome> m_chunks;
	};
}
