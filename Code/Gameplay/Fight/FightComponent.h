#pragma once
#include "../../ska/ECS/Component.h"
#include "../../ska/ECS/ECSDefines.h"

class FightComponent : public ska::Component {
public:
	FightComponent() {
		level = 0;
		id = 0;
		fighter = 0;
	}

	unsigned int level;
	int id;
	ska::EntityId fighter;
};