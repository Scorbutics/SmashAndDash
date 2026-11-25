#pragma once
#include <cstdint>
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

	Start StartOf(const WorldPlan& plan);

	// A world of 8 x 8 chunks from a seed, written in folder (worldPath : as the maps name it, ends with '/')
	Start GenerateWorld(std::int64_t seed, const std::string& folder, const std::string& worldPath, const Resources& resources = {});
}
