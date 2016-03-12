#pragma once
#include "../../ska/ECS/Component.h"
class GravityAffectedComponent : public ska::Component {
public:
	float friction;
	float weight;
};