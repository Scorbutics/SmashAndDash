#pragma once
#include "../Data/RawStatistics.h"

class BattleComponent : public RawStatistics<int> {
public:
	BattleComponent() {
		hp = 10;
	}

	RawStatistics<int> variations;
};
