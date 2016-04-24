#pragma once
#include "../../ska/ECS/Component.h"

class SkillComponent : public ska::Component {
public:
	int damage;
	int knockback;
	float speed;

	int noise;
	unsigned int period;
	
};
