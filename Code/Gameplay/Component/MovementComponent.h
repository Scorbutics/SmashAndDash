#pragma once
#include "../../ska/ECS/Component.h"
class MovementComponent : public ska::Component {
public:
	float ax;
	float ay;
	float az;

	float vx;
	float vy;
	float vz;
};