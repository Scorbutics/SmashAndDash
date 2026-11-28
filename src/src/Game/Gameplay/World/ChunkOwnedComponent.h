#pragma once
#include <string>
#include <vector>
#include "Core/ECS/Component.h"

// A character of the map (Character.addCharacter) : it belongs to the chunk it stands in, saved and brought back with it
// (ChunkEntities). What it was made with, kept to make it again. The wild Pokemon (WildPokemonComponent) belong to their
// chunk too.
struct ChunkOwnedComponent {
	int sprite = 0;
	std::string name;
	std::string script;
	std::vector<std::string> arguments;
	// Nothing pushes it (Character.anchor)
	bool anchored = false;
	bool animatedWhenStill = false;
};

SKA_DECLARE_COMPONENT(ChunkOwnedComponent);
