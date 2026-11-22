#pragma once
#include <string>
#include "Core/ECS/Component.h"
#include "../Data/RawStatistics.h"

class BattleComponent : public RawStatistics<int> {
public:
	BattleComponent() {
		hp = 10;
	}

	RawStatistics<int> variations;
	int maxHp = 10;
	int level = 1;
	// Data/Monsters/<monster>.ini
	int monster = 0;
	// Its types ([Description] type1, type2 : "Aucun" when it has one type)
	std::string type1 = "Normal";
	std::string type2 = "Aucun";
};

SKA_DECLARE_COMPONENT(BattleComponent);
