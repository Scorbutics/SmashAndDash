#pragma once
#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "Terrain.h"

// The infinite world (the C++ port of tools/worldgen/infinite.py, bit for bit : tests/worldgen/infinite.txt) : a
// continuous world generated region by region as the player walks. Every value is a pure function of the block (the
// blocks of the game : the chunks 0 .. WorldChunks - 1, the start at the centre) and of the seed : a region generated
// alone is the same as with its neighbours, in any order. The stages : the climate (fixed rank tables, a local majority
// filter of the biomes), the continents and the ocean, the villages (one at most per cell of a lattice), the roads
// (each village to its nearest ones, an A* in a box around the two), the terrain, its terraces cleaned and their stairs
// (passes of a bounded reach, in a window around the blocks asked).
namespace worldgen {
	constexpr int RegionChunks = 8;
	constexpr int Region = RegionChunks * Chunk;
	constexpr int WorldChunks = 512;
	constexpr int Center = WorldChunks / 2 * Chunk;
	constexpr int VillageCell = 144;
	constexpr int RoadCells = 2;

	// The kinds of sea of a block (none : the land, or a lake)
	enum class Sea : std::uint8_t { None = 0, Shallows = 1, Deep = 2, Reef = 3, Bar = 4 };

	// The share of the world under a value of fbm (3 or 4 octaves : RANK3, RANK4)
	double Rank3(double v);
	double Rank4(double v);
	// The seed of a cell of the lattice of the villages (a 31 bits hash)
	std::int64_t CellSeed(std::int64_t seed, std::int64_t i, std::int64_t j);
	// Floor division and modulo (the blocks are positive in the game : the same as Python for any integer)
	inline int FloorDiv(int a, int b) { return a / b - ((a % b != 0) && ((a < 0) != (b < 0)) ? 1 : 0); }
	inline int FloorMod(int a, int b) { return a - FloorDiv(a, b) * b; }

	using Cell2 = std::pair<int, int>;

	// A village of the infinite world : in a cell of the lattice
	struct CellVillage {
		CellVillage(Village village) : v(std::move(village)) {}
		Village v;
		bool first = false;
		Cell2 cell;
		// The distance to the village up to 8 blocks (its ground levelled), up to 12 (its biome)
		std::unordered_map<std::uint64_t, int> near;
		std::map<Block, int> nearBiome;
		std::unordered_map<std::uint64_t, int> nearBiomeFast;
	};

	inline std::uint64_t Key(int x, int y) { return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(x)) << 32) | static_cast<std::uint32_t>(y); }

	// The terrain of blocks (cleaned), their stairs (sorted by rows : y then x), the terraces out of reach
	struct Area {
		int x0 = 0, y0 = 0, width = 0, height = 0;
		std::vector<Cell> terrain;     // by rows
		std::vector<Block> stairs;     // (x, y) blocks of the world, by rows
		int unreached = 0;
		const Cell& at(int x, int y) const { return terrain[static_cast<std::size_t>(y - y0) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x - x0)]; }
	};

	class Infinite {
	public:
		explicit Infinite(std::int64_t seed);
		~Infinite();
		Infinite(const Infinite&) = delete;
		Infinite& operator=(const Infinite&) = delete;

		std::int64_t seed() const { return m_seed; }

		// The climate : 0 the open sea .. 1 the land ; 0 flat .. 1 mountains ; the biome of the climate, before the
		// smoothing ; the biome before the villages ; the biome of a block
		double continent(int x, int y) const;
		double relief(int x, int y) const;
		Biome climateAt(int x, int y);
		Biome baseBiome(int x, int y);
		Biome biome(int x, int y);

		// The height of a block (levelled around a village), before the villages ; its kind of sea
		double height(int x, int y);
		double landHeight(int x, int y);
		Sea ocean(int x, int y);

		// The villages : the site of a cell and its biome, the cell of the first village, the village of a cell, of the
		// cell of a block (nullptr : none)
		std::optional<std::pair<Block, Biome>> site(int i, int j);
		Cell2 start();
		const CellVillage* village(int i, int j);
		const CellVillage* villageOf(int x, int y) { return village(FloorDiv(x, VillageCell), FloorDiv(y, VillageCell)); }

		// The roads : the nearest villages of a cell, its links (both ways), the box of a road, its blocks (nullptr : no
		// way), its middle (from the smaller cell), the roads of the villages of cells, of a cell
		const std::vector<Cell2>& nearest(int i, int j);
		std::vector<Cell2> links(int i, int j);
		std::array<int, 4> roadBox(const Village& a, const Village& b) const;
		const Blocks* road(Cell2 a, Cell2 b);
		const std::vector<Block>* roadPath(Cell2 a, Cell2 b);
		std::vector<std::pair<Cell2, Cell2>> roads(int ci0, int cj0, int ci1, int cj1);
		const std::unordered_set<std::uint64_t>& roadsOfCell(int i, int j);
		bool onRoad(int x, int y);
		bool nearRoad(int x, int y, int limit = 2);

		// The ground of a block and its level (before the cleaning)
		Cell classify(int x, int y);
		// The blocks x0 .. x1 - 1, y0 .. y1 - 1 : cleaned, their stairs (a window Apron blocks wider)
		Area area(int x0, int y0, int x1, int y1);

		// The values kept (the caches), forgotten (computed again when asked : pure functions) ; no village or road
		// given before may be kept by the caller
		std::size_t cached() const;
		void forget();

	private:
		struct RoadEntry {
			bool done = false;
			std::optional<Blocks> blocks;
			std::optional<std::vector<Block>> path;
		};

		std::int64_t m_seed;
		double m_ox, m_oy;
		std::unordered_map<std::uint64_t, Biome> m_climate;
		std::unordered_map<std::uint64_t, Biome> m_base;
		std::map<Cell2, std::unique_ptr<CellVillage>> m_villages;
		std::map<std::pair<Cell2, Cell2>, RoadEntry> m_roads;
		std::map<Cell2, std::unordered_set<std::uint64_t>> m_cellRoads;
		std::map<Cell2, std::vector<Cell2>> m_nearest;
		std::optional<Cell2> m_start;
	};
}
