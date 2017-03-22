#pragma once
#include <vector>
#include "../../ska/Graphic/Rectangle.h"

namespace ska {
	class IniReader;
}

class MobSpawner {
public:
	MobSpawner() = default;
	virtual ~MobSpawner() = default;

	virtual std::vector<ska::IniReader>& getMobSettings() = 0;
	virtual int spawnMob(ska::Rectangle pos, unsigned int rmin, unsigned int rmax, unsigned int nbrSpawns, ska::IniReader* dataSpawn) = 0;
};
