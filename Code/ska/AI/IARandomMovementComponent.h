#pragma once
#include "../ECS/Component.h"
namespace ska {
	class IARandomMovementComponent : public Component {
	public:
		IARandomMovementComponent() {
			lastTimeStarted = 0;
			delay = 0;
			direction = 0;
		}

		int direction;
		unsigned int delay;
		unsigned int lastTimeStarted;
	};
}