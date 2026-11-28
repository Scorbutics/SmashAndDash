#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Infinite.h"
#include "Layers.h"

// A region of the infinite world built as a world (the C++ port of tools/worldgen/infinite.py RegionWorld) : the stages
// of the finite world (World : the grounds, the cliffs, the villages decorated, the trees, the places beside the roads,
// the signs, the items, the rooms) on a window BuildMargin blocks around the region, its terrain known PlanMargin blocks
// around (cleaned, its stairs : Infinite::area) ; the villages and the roads that reach it, the biomes blended.
namespace worldgen {
	constexpr int BuildMargin = 2 * Chunk;
	constexpr int PlanMargin = BuildMargin + 60;

	class RegionBuilder {
	public:
		RegionBuilder(Infinite& world, int rx, int ry, int margin = BuildMargin, int plan = PlanMargin - BuildMargin);
		~RegionBuilder();

		int rx() const { return m_rx; }
		int ry() const { return m_ry; }
		const WorldPlan& plan() const { return *m_plan; }
		const Terrain& terrain() const { return *m_terrain; }
		const RegionOptions& options() const { return m_options; }
		// The cells of the villages of the plan (the order of the villages of the plan)
		const std::vector<Cell2>& villageCells() const { return m_cells; }
		// The reefs of the terrain known (rocks out of the water)
		int reefs() const { return m_reefs; }
		// The world of the region (its tiles)
		std::unique_ptr<World> build(const Shapes& shapes, const std::vector<RoomTemplate>& templates) const;

	private:
		void biomeField(Infinite& world);
		void planPois(Infinite& world);

		int m_rx, m_ry;
		RegionOptions m_options;
		std::unique_ptr<WorldPlan> m_plan;
		std::unique_ptr<Terrain> m_terrain;
		std::vector<Cell2> m_cells;
		int m_reefs = 0;
	};

	// The region of a block of the world
	inline Cell2 RegionOf(int x, int y) { return { FloorDiv(x, Region), FloorDiv(y, Region) }; }
}
