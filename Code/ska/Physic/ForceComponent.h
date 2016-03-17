#pragma once
#include "../ECS/Component.h"

namespace ska {
	class ForceComponent : public ska::Component {
	public:
		int x;
		int y;
		int z;
	};
}
