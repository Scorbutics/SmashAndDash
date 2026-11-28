#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include "Chipset.h"

// The files of a generated world (the C++ port of tools/worldgen : World.write, write_spawns, rooms.light) : a folder per
// map (the chunks w_<x>_<y> and the rooms w_room_<n> : its three layers, its light, its .ini, its events, its wild
// Pokemon), a chipset per map (Chipsets), as the game reads them (SCRIPTING.md, "The world").
namespace worldgen {
	// Where the generator finds its data : Resources/Worldgen (shapes.txt, tiles.png, tiles.txt, rooms),
	// Resources/Chipsets (corr.png, tile_scripts)
	struct Resources {
		std::string root = "./Resources";
	};

	// The format of the generated worlds : a save whose world is older is generated again (the same seed, SaveSlots).
	// 2 : the depth of the tiles in the properties of the chipsets (.prop) ; 3 : the hideouts of the wild Pokemon there ;
	// 4 : the front leaves of the tall grass (over the characters in it) ; 5 : the texts of the events in UTF-8 ; 6 : no
	// front leaves, the tall grass flagged in the chipsets (the legs of the characters in it are not drawn) ; 7 : no tree
	// canopy over the top of another tree ; 8 : the canopies and the boulders flagged (hideouts) ; 9 : the collision masks of
	// the tiles (.png.mask : the walls are the solid parts of the tiles, not their whole blocks) ; 10 : the maps of the
	// outdoors played as one (no teleport on their edges, the items placed on their blocks of the world) ; 11 : the infinite
	// world (InfiniteFiles.h : its chunks written as the player comes near them)
	constexpr int WorldVersion = 11;
	constexpr int InfiniteWorldVersion = 11;

	// Where the player starts : the block below the centre of the first village
	struct Start {
		std::string map;
		int x = 0;
		int y = 0;
	};

	// The light of a room, one value per block corner : the shade of the back wall and of the furniture
	RgbImage RoomLight(const Room& room);

	// Writes the world in folder (replaced) ; worldPath : the folder as the maps name it (the "file" of their chipset)
	void WriteWorld(const World& world, TileImages& images, const RgbImage& corr, const std::string& tileScripts,
		const std::string& folder, const std::string& worldPath);

	// A chunk of a region of the infinite world (Region.h), its rooms (their chipset : chipset_rooms_<rx>_<ry>), written in
	// folder (kept : the other regions are there)
	void WriteRegionChunk(const World& world, TileImages& images, const RgbImage& corr, const std::string& tileScripts,
		const std::string& folder, const std::string& worldPath, int cx, int cy);
	void WriteRegionRooms(const World& world, TileImages& images, const RgbImage& corr, const std::string& tileScripts,
		const std::string& folder, const std::string& worldPath, const std::string& chipset);

	Start StartOf(const WorldPlan& plan);

	// A world of 8 x 8 chunks from a seed, written in folder (worldPath : as the maps name it, ends with '/'), step by
	// step (a step : a few hundred milliseconds at most ; the screen can be drawn between two of them)
	class WorldWriter {
	public:
		WorldWriter(std::int64_t seed, std::string folder, std::string worldPath, Resources resources = {});
		~WorldWriter();
		// Does the next step : false once the world is written
		bool step();
		bool done() const { return m_step >= steps(); }
		// 0 .. 1
		double progress() const { return static_cast<double>(m_step) / steps(); }
		// What the next step does (French, UTF-8 as the texts of the game : the loading screen)
		std::string stage() const;
		// Where the player starts (once done)
		const Start& start() const { return m_start; }
	private:
		struct State;
		int steps() const;

		std::int64_t m_seed;
		std::string m_folder, m_worldPath;
		Resources m_resources;
		std::unique_ptr<State> m_state;
		int m_step = 0;
		Start m_start;
	};

	// A world written at once (WorldWriter)
	Start GenerateWorld(std::int64_t seed, const std::string& folder, const std::string& worldPath, const Resources& resources = {});
}
