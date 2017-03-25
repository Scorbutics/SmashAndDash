#pragma once
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "ECS/Component.h"
#include "Point.h"
#include "Utils/TimeUtils.h"

namespace ska {
	class IADefinedMovementComponent : public Component {
	public:
		IADefinedMovementComponent() {
			delay = 0;
			directionIndex = 0;
			loop = false;
			ghost = false;
			callbackActive = false;
			lastTimeStarted = TimeUtils::getTicks();
		}

		bool loop;
		bool ghost;
		unsigned int directionIndex;
		std::vector<Point<int>> directions;
		unsigned int delay;
		Point<int> origin;
		unsigned int lastTimeStarted;

		bool callbackActive;
		ScriptSleepComponent callback;
	};
}
