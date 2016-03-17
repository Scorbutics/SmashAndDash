#pragma once
#include "../ECS/Component.h"

namespace ska {
	class HitboxComponent : public ska::Component {
	public:
		int xOffset;
		int yOffset;
		unsigned int width;
		unsigned int height;
	};
}