#pragma once
#include "../../ska/ECS/Component.h"
class FightComponent : public ska::Component {
public:
	unsigned int level;
	int id;
};