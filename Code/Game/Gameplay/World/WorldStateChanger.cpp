#include "WorldStateChanger.h"

WorldStateChanger::WorldStateChanger(WorldState& ws, const std::string& fileName, const std::string& chipsetName, const bool sMap, ska::Point<int> screenSize_) :
	screenSize(screenSize_),
	worldState(ws),
	worldFileName(fileName),
	worldChipsetName(chipsetName),
	sameMap(sMap) {

}
