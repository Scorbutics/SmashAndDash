#pragma once
#include "../../ska/ECS/Component.h"
#include "../../ska/ECS/ECSDefines.h"

class FightComponent : public ska::Component {
public:
	FightComponent() {
		level = 0;
		scriptId = 0;
		fighterOpponent = 0;
		fighterPlayer = 0;
	}

	unsigned int level;
	int scriptId;
	ska::EntityId fighterOpponent;
	ska::EntityId fighterPlayer;
};