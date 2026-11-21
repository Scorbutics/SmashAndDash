#pragma once
#include "Core/ECS/Component.h"
#include "Core/Utils/TimeUtils.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/Point.h"

class SkillComponent {
public:
	SkillComponent(): range(0), battler(0), amplitude(0) {
		damage = 0;
		knockback = 0;
		speed = 0.01F;
		noise = 0;

		lastTime = ska::TimeUtils::getTicks();
		currentTime = lastTime;
		firstTime = lastTime;
	}

	int damage;
	int knockback;

	unsigned int range;

	int noise;

	ska::EntityId battler;
	ska::Point<int> origin;
	ska::Point<int> target;
	unsigned int currentTime;
	unsigned int lastTime;
	unsigned int firstTime;

	float speed;
	float amplitude;
};

SKA_DECLARE_COMPONENT(SkillComponent);
