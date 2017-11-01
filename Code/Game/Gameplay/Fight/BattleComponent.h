#pragma once
#include "ECS/SerializableComponent.h"
#include "../Data/RawStatistics.h"

class BattleComponent : public ska::SerializableComponent, public RawStatistics<int> {
public:
	BattleComponent() {
		hp = 10;
	}

	RawStatistics<int> variations;
};
