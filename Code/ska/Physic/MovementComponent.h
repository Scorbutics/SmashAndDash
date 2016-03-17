#pragma once
#include "../ECS/Component.h"
namespace ska {
	struct MovementComponent : public ska::Component {
		float ax;
		float ay;
		float az;

		float vx;
		float vy;
		float vz;
	};
}
