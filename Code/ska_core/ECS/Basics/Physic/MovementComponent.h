#pragma once
#include "../../Component.h"
namespace ska {
	struct MovementComponent : public Component {
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
