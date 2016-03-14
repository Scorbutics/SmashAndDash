#pragma once
#include "../../ska/ECS/Component.h"
struct MovementComponent : public ska::Component {
	float ax;
	float ay;
	float az;

	float vx;
	float vy;
	float vz;
};