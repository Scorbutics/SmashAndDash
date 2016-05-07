#pragma once
#include "../ECS/Component.h"
namespace ska {
	struct MovementComponent : public ska::Component {
		MovementComponent() {
			ax = 0;
			ay = 0;
			az = 0;

			vx = 0;
			vy = 0;
			vz = 0;
		}
		float ax;
		float ay;
		float az;

		float vx;
		float vy;
		float vz;
	};
}
