#include "WorldStateChanger.h"

WorldStateChanger::WorldStateChanger(WorldState& ws, const std::string& fileName, const std::string& chipsetName, const bool sMap) :
	worldState(ws),
	worldFileName(fileName),
	worldChipsetName(chipsetName),
	sameMap(sMap) {
	
}