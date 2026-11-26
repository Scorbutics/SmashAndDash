#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace worldgen {
	struct Start;
}

// The saves of the player (SaveDirectory) : the list of the title screen, a new game (a save made from
// Data/Saves/new_game, its world to generate : [World] generate, seed), the world of a save generated in it.
namespace SaveSlots {
	struct Slot {
		std::string name;
		std::string map;   // where the trainer is (empty : a world still to generate)
		long long played;  // when it was last written (the order of the list : the most recent first)
	};

	// The saves (not new_game), the most recent first
	std::vector<Slot> List();
	bool Exists(const std::string& name);
	// A free name for a new save : "partie1", "partie2"...
	std::string NewName();
	// The seed of a new world : SAD_WORLD_SEED (environment), else drawn
	std::int64_t NewSeed();
	// Makes the save from Data/Saves/new_game, its world to generate from the seed
	void Create(const std::string& name, std::int64_t seed);
	// The seed of the world to generate of a save (a new one, or one not there anymore : a save of before names
	// ./Data/Worlds/seed<N>/), else nothing (its world is there)
	std::optional<std::int64_t> WorldToGenerate(const std::string& name);
	// The folder of the world of a save (<save>/World/), as its maps name it
	std::string WorldPath(const std::string& name);
	// The world of the save is written : the save names it ; a new world : the trainer starts in its first village
	void WorldGenerated(const std::string& name, std::int64_t seed, const worldgen::Start& start);
}
