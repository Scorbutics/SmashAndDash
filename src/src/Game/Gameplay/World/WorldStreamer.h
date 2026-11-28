#pragma once
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "Core/Point.h"
#include "World/Tileset.h"
#include "World/TilesetCorrespondanceMapper.h"
#include "World/LightMap.h"
#include "Chunks/ChunkGrid.h"
#include "Chunks/ChunkPrepareQueue.h"

namespace ska {
	class DrawableContainer;
	namespace cp {
		class Space;
	}
}
class WorldImpl;
class ChunkEntities;
namespace chunks {
	class Generator;
}

// The chunks of the world loaded around the player : their images are prepared away from the game (a thread, a web
// worker), their map made from them (a step for its tileset, a step for the rest : one chunk at a time), with their
// walls, water edges, light and entities. The chunks far from the player leave.
class WorldStreamer {
public:
	static constexpr unsigned int BlockSize = 48;

	WorldStreamer(WorldImpl& world, const ska::TilesetCorrespondanceMapper& mapper, ChunkEntities& entities);
	~WorldStreamer();

	// The world of a map (its outdoors, or this room alone), its chunks around a block of this map loaded at once ; the
	// block in the world is given. The chunks of the world before are left first (leave)
	ska::Point<int> enter(const std::string& map, ska::Point<int> localBlock, ska::cp::Space& space);
	// All the chunks leave (their entities kept)
	void leave();
	// A step : the chunks to come around the player (its block in the world), the ones to leave
	void update(ska::Point<int> block);
	// A world was entered
	bool entered() const { return m_space != nullptr; }

	const chunks::Grid& grid() const { return m_grid; }
	// The map (the chunk) of a block of the world
	std::string mapAt(ska::Point<int> block) const { return m_grid.name(m_grid.at(block.x, block.y)); }
	// The block of the world of the first block of a map ; (0, 0) for a map of another world
	ska::Point<int> originOf(const std::string& map) const;
	// The tileset of the chunk of a block, the one of the player when it is not there
	const ska::Tileset& tilesetAt(ska::Point<int> block) const;
	// The chunks loaded : their maps and their blocks in the world
	std::vector<std::pair<std::string, ska::Rectangle>> loaded() const;

	void updateAnimations();
	void graphicUpdate(const ska::Rectangle& camera, ska::DrawableContainer& drawables);

private:
	struct Chunk {
		std::string name;
		ska::TilesetPtr tileset;
		ska::LightMap light;
		std::vector<std::size_t> walls;
		std::vector<std::size_t> water;
	};

	// Kept once found (a chunk's map is read to know its chipset)
	const chunks::Paths& pathsOf(chunks::Coordinates c);
	ska::Rectangle blocksOf(chunks::Coordinates c) const;
	// A chunk with its tileset, its map not on the world yet
	struct Staged {
		chunks::Coordinates at;
		std::unique_ptr<Chunk> chunk;
		chunks::Images images;
	};

	// A chunk put on the map, in two parts : its tileset, then the rest
	Staged stage(chunks::Coordinates c, chunks::Images images);
	void commit(Staged staged);
	void unload(chunks::Coordinates c);
	void loadNow(chunks::Coordinates c);
	// An infinite world : its chunks written as the player comes near them
	void openGenerator();
	bool generating() const { return m_generator != nullptr && m_grid.single.empty(); }
	bool ready(chunks::Coordinates c);
	void buildWalls(chunks::Coordinates c, Chunk& chunk);
	// The outline of the water of a chunk (the player surfs in it : touching it starts or stops surfing), on the edges
	// between water and ground, the ones with the chunks around too : made again when they come or leave
	void buildWater(chunks::Coordinates c);
	void buildWaterAround(chunks::Coordinates c);
	bool water(int x, int y) const;
	void settings(ska::Point<int> block);

	WorldImpl& m_world;
	const ska::TilesetCorrespondanceMapper& m_mapper;
	ChunkEntities& m_entities;
	ska::cp::Space* m_space = nullptr;

	chunks::Grid m_grid{ 1, 1, 1, 1, {} };
	std::map<chunks::Coordinates, std::unique_ptr<Chunk>> m_chunks;
	// Asked to the queue (by request id), prepared and not on the map yet
	std::unique_ptr<chunks::PrepareQueue> m_queue;
	std::map<std::uint64_t, chunks::Coordinates> m_pending;
	std::map<chunks::Coordinates, chunks::Images> m_ready;
	std::optional<Staged> m_staged;
	std::uint64_t m_lastRequest = 0;
	// The files of the chunks of this world
	std::map<chunks::Coordinates, chunks::Paths> m_paths;
	std::optional<chunks::Coordinates> m_current;
	std::unique_ptr<chunks::Generator> m_generator;
};
