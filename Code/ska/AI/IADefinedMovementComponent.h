#pragma once
#include "../Script/ScriptSleepComponent.h"
#include "../ECS/Component.h"
#include "../Graphic/Point.h"
#include "../Utils/TimeUtils.h"
namespace ska {
	class IADefinedMovementComponent : public Component {
	public:
		IADefinedMovementComponent() {
			lastTimeStarted = 0;
			delay = 0;
			directionIndex = 0;
			loop = false;
			lastDirectionDiff = 0;
			callbackActive = false;
			lastTimeStarted = ska::TimeUtils::getTicks();
		}

		bool loop;
		unsigned int directionIndex;
		std::vector<ska::Point<int>> directions;
		unsigned int delay;
		ska::Point<int> origin;
		ska::Point<int> lastDirectionDiff;
		unsigned int lastTimeStarted;

		bool callbackActive;
		ScriptSleepComponent callback;
	};
}