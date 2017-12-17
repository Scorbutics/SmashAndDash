/*#pragma once
#include <string>
#include "Point.h"

class WorldState;

struct WorldStateChanger {
	WorldStateChanger(WorldState& ws, const std::string& fileName, const std::string& chipsetName, const bool sameMap, ska::Point<int> screenSize);
	~WorldStateChanger() = default;

	const ska::Point<int> screenSize;
	WorldState& worldState;
	const std::string& worldFileName;
	const std::string& worldChipsetName;
	const bool sameMap;
	
};*/