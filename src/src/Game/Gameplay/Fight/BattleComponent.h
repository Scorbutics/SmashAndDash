#pragma once
#include "Core/ECS/Component.h"
#include "../Data/RawStatistics.h"

class BattleComponent : public RawStatistics<int> {
public:
	BattleComponent() {
		hp = 10;
	}

	RawStatistics<int> variations;
};

SKA_DECLARE_COMPONENT(BattleComponent);
