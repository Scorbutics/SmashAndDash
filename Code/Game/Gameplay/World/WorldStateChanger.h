#pragma once
#include <string>

class WorldState;

struct WorldStateChanger {
	WorldStateChanger(WorldState& ws, const std::string& fileName, const std::string& chipsetName, const bool sameMap);
	~WorldStateChanger() = default;

	WorldState& worldState;
	const std::string& worldFileName;
	const std::string& worldChipsetName;
	const bool sameMap;
	
};