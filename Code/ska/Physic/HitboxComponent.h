#pragma once
#include "../ECS/Component.h"

namespace ska {
	struct HitboxComponent : public ska::Component {
	public:
		int xOffset;
		int yOffset;
		unsigned int width;
		unsigned int height;
	};
}