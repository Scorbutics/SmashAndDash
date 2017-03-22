#pragma once
#include "../../ska/ECS/Component.h"
#include "../Data/RawStatistics.h"

class BattleComponent : public ska::Component, public RawStatistics<int> {
public:
	BattleComponent() {
		hp = 10;
	}

	RawStatistics<int> variations;
};
