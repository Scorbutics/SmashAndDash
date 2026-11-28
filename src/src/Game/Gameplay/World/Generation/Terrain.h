#pragma once
#include <array>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <vector>
#include "Villages.h"

// The terrain of a generated world (the C++ port of tools/worldgen : World.plan_routes, astar, height, classify, clean,
// place_stairs, plan_pois) : the roads between the villages, the ground of each block and its level (the terraces),
// the stairs, the places beside the roads.
namespace worldgen {
	// The grounds (the order of the reference values : tools/worldgen/golden.py MATS)
	// (Reef : the rocks in the shallows of the infinite world, Infinite.h)
	enum class Ground : std::uint8_t { Water = 0, Sand = 1, Path = 2, Grass = 3, Pave = 4, Reef = 5 };
	constexpr int MaxLevel = 4;
	constexpr int PoiSpacing = 35;

	struct Cell {
		Ground ground = Ground::Grass;
		int level = 0;
		bool operator==(const Cell& other) const { return ground == other.ground && level == other.level; }
		bool operator!=(const Cell& other) const { return !(*this == other); }
	};

	struct Route {
		int from, to;
		std::vector<Block> path;
	};

	struct Poi {
		std::string kind; // "camp", "ruins", "rest"
		int x, y, w, h;
		std::vector<Block> spur;
		Biome biome;
	};

	// The 8 neighbours : N E S W, then NE SE SW NW
	constexpr std::array<std::array<int, 2>, 8> D8{ { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 }, { 1, -1 }, { 1, 1 }, { -1, 1 }, { -1, -1 } } };

	// The neighbours of a block for which a test holds : 8 bits (N E S W NE SE SW NW), a diagonal only when its two sides
	// do not
	template <class Test>
	std::array<bool, 8> Mask8(Test cellIs, int x, int y) {
		auto bits = std::array<bool, 8>{};
		for (auto k = 0u; k < 8; k++) {
			bits[k] = cellIs(x + D8[k][0], y + D8[k][1]);
		}
		constexpr std::array<std::array<std::size_t, 2>, 4> sides{ { { 0, 1 }, { 2, 1 }, { 2, 3 }, { 0, 3 } } };
		for (auto k = 4u; k < 8; k++) {
			if (bits[sides[k - 4][0]] || bits[sides[k - 4][1]]) {
				bits[k] = false;
			}
		}
		return bits;
	}

	class Terrain {
	public:
		explicit Terrain(const WorldPlan& plan);

		const WorldPlan& plan() const { return m_plan; }
		// The blocks of the terrain : the world, or a window of the infinite world (RegionBuilder)
		int x0() const { return m_x0; }
		int y0() const { return m_y0; }
		int width() const { return m_width; }
		int height() const { return m_height; }
		bool inside(int x, int y) const { return m_x0 <= x && x < m_x0 + m_width && m_y0 <= y && y < m_y0 + m_height; }
		// The height of a block (the noise, the biomes, the mountains, the island, the levelled villages)
		double heightAt(int x, int y) const;
		const Blocks& routes() const { return m_routes; }
		const std::vector<Route>& routePaths() const { return m_routePaths; }
		const std::map<Block, int>& nearRoute() const { return m_nearRoute; }
		Cell cell(int x, int y) const { return inside(x, y) ? m_terrain[index(x, y)] : Cell{ Ground::Water, 0 }; }
		// The ground and the level of a block, out of the map : water, level 0
		Ground ground(int x, int y) const;
		int level(int x, int y) const;
		const Blocks& stairs() const { return m_stairs; }
		const std::vector<Poi>& pois() const { return m_pois; }

		// The terraces : the lower neighbours of a block, the foot of a face, a block walked on without a cliff
		std::array<bool, 8> lowMask(int x, int y) const;
		bool isFoot(int x, int y) const;
		bool flat(int x, int y) const;
		bool stairOk(int x, int y) const;
		bool inTown(int x, int y, int r) const;

	private:
		friend class RegionBuilder;
		Terrain(const WorldPlan& plan, int x0, int y0, int width, int height) : m_plan(plan), m_x0(x0), m_y0(y0), m_width(width), m_height(height) {}
		std::size_t index(int x, int y) const { return static_cast<std::size_t>(y - m_y0) * static_cast<std::size_t>(m_width) + static_cast<std::size_t>(x - m_x0); }
		void planRoutes();
		std::optional<std::vector<Block>> astar(const std::vector<double>& cost, Block start, Block goal) const;
		Cell classify(int x, int y) const;
		void clean();
		bool placeStairs();
		bool poiBlockOk(int x, int y) const;
		std::optional<std::tuple<int, int, std::vector<Block>>> poiArea(const std::string& kind, int x, int y, int nx, int ny) const;
		void planPois();

		const WorldPlan& m_plan;
		int m_x0 = 0, m_y0 = 0, m_width = 0, m_height = 0;
		Blocks m_routes;
		std::vector<Route> m_routePaths;
		std::map<Block, int> m_nearRoute;
		Blocks m_villageGround;
		std::vector<Cell> m_terrain;
		Blocks m_stairs;
		std::vector<Poi> m_pois;
	};
}
