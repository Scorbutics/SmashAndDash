#pragma once
#include "../ECS/Component.h"

namespace ska {
	class PositionComponent : public ska::Component {
	public:
		int x;
		int y;
		int z;
	};
}

