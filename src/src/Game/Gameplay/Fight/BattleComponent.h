#pragma once
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
};

SKA_DECLARE_COMPONENT(BattleComponent);
