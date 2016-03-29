#pragma once
#include "../ECS/Component.h"
namespace ska {
	class IAMovementComponent : public Component {	
	public:
		IAMovementComponent() {
			lastTimeStarted = 0;
			delay = 0;
			direction = 0;
			currentPathIndex = 0;
			type = 0;
		}

		std::string path;
		unsigned int currentPathIndex;
		int direction;
		int type;
		unsigned int delay;
		unsigned int lastTimeStarted;
	};
}