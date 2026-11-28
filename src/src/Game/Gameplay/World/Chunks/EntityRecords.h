#pragma once
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

// The entities of the chunks that are not loaded, as Minecraft keeps them (plain C++, tests/chunks) : a chunk saves its
// entities when it leaves and brings them back when it comes again (ChunkEntities). The game saves them with the save
// (<save>/Chunks.txt).
namespace chunks {
	struct EntityRecord {
		enum class Kind { Wild, Character };
		Kind kind = Kind::Wild;
		// Its block in its chunk
		int x = 0;
		int y = 0;
		int facing = 0;

		// A wild Pokemon
		int monster = 0;
		int level = 1;
		std::string attitude = "passive";
		// It walks (GridMovementComponent)
		int pathType = 0;
		// A bird : the time left before it flies away (ms), -1 : it stays
		int stay = -1;
		unsigned int calm = 0;

		// A character (Character.addCharacter)
		int sprite = 0;
		std::string name;
		std::string script;
		std::vector<std::string> arguments;
		// "", "wander" or "path" (its directions)
		std::string movement;
		std::vector<int> path;
		int layer = 0;
		bool anchored = false;
		bool animatedWhenStill = false;
	};

	struct ChunkRecord {
		// The game time (EntityStore::time) when the chunk left
		long long left = 0;
		std::vector<EntityRecord> entities;
	};

	// A chunk with a record ran its loading events already : they do not run again (its entities come from the record)
	struct EntityStore {
		// The time played (ms)
		long long time = 0;
		std::map<std::string, ChunkRecord> chunks;

		void write(std::ostream& out) const;
		// The lines not understood are left out
		void read(std::istream& in);
	};
}
