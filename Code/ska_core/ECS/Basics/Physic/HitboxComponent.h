#pragma once
#include "../../Component.h"

namespace ska {
	struct HitboxComponent : public Component {
	public:
		int xOffset;
		int yOffset;
		unsigned int width;
		unsigned int height;
	};
}
