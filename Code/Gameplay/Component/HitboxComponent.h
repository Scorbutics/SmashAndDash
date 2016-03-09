#pragma once
#include "../../ska/ECS/Component.h"
class HitboxComponent : public ska::Component {
public:
	int xOffset;
	int yOffset;
	unsigned int width;
	unsigned int height;
};
