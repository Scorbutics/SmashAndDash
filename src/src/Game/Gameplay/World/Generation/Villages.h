#pragma once
#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "Climate.h"

// The villages of a generated world, the distance to them, the biome weights of each block and the mixes of the corners
// (the C++ port of tools/worldgen : Rng, Village, World.village_site, plan_villages, distance_to, biome_field).
namespace worldgen {
	using Block = std::pair<int, int>;
	using Blocks = std::set<Block>;

	constexpr int VillageSpacing = 90;
	constexpr int MixSteps = 6;

	// A seeded random sequence (the same plan for the same seed)
	class Rng {
	public:
		explicit Rng(std::int64_t seed) : m_seed(seed) {}
		double random();
		int randint(int a, int b);
		// An index in a list of "count" items
		std::size_t choice(std::size_t count);
		std::int64_t seed() const { return m_seed; }
	private:
		std::int64_t m_seed;
		std::int64_t m_n = 0;
	};

	// The villages of a biome : their roofs, the words of their names, the line of their signs (UTF-8)
	struct VillageStyle {
		std::vector<std::string> roofs;
		std::vector<std::string> words;
		std::string line;
	};
	const VillageStyle& StyleOf(Biome biome);

	struct Building {
		std::string kind;   // "center", "house"
		std::string roof;   // "" for none (the centre)
		int x0, y0, w, h, door, doorRow;
	};

	// A village grown like the villages of Minecraft : roads from a plaza, buildings along them
	struct Village {
		Village(int cx, int cy, Rng& rng, const std::vector<std::string>& roofs, int housesMin, int housesMax);
		// The buildings as the Python generator prints them (repr of its list of tuples)
		std::string buildingsRepr() const;

		Block center;
		Blocks road, footprint, flowers, yard, pond, pondRing, cells;
		std::vector<Building> buildings;
		Biome biome = Biome::Meadow;
		std::string name; // UTF-8
	};

	// The distance (blocks, 4 neighbours) to the nearest of the cells, up to limit
	std::map<Block, int> DistanceTo(const Blocks& cells, int limit);

	// The plan of a world before its terrain : its climate, its villages, the biomes of its blocks blended
	class WorldPlan {
	public:
		WorldPlan(std::int64_t seed, int chunksX, int chunksY);

		const Climate& climate() const { return m_climate; }
		Block town() const { return m_town; }
		const std::vector<Village>& villages() const { return m_villages; }
		const std::map<Block, int>& nearVillage() const { return m_nearVillage; }
		// The weight of a biome on a block (0 .. 1)
		double weight(int x, int y, Biome biome) const { return m_weights[index(x, y) * BiomeCount + static_cast<std::size_t>(biome)]; }
		// The mix of the biomes at a corner of the blocks (i, j : 0 .. width, 0 .. height) : MixSteps shares
		const std::array<std::uint8_t, BiomeCount>& corner(int i, int j) const { return m_corners[static_cast<std::size_t>(j) * static_cast<std::size_t>(m_climate.width() + 1) + static_cast<std::size_t>(i)]; }

	private:
		std::size_t index(int x, int y) const { return static_cast<std::size_t>(y) * static_cast<std::size_t>(m_climate.width()) + static_cast<std::size_t>(x); }
		Block villageSite() const;
		void planVillages();
		void biomeField();

		std::int64_t m_seed;
		Climate m_climate;
		Block m_town;
		std::vector<Village> m_villages;
		std::map<Block, int> m_nearVillage;
		std::map<Block, Biome> m_villageBiome;
		std::vector<double> m_weights;
		std::vector<std::array<std::uint8_t, BiomeCount>> m_corners;
	};
}
