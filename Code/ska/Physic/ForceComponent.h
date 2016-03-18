#pragma once
#include "../ECS/Component.h"

namespace ska {
	class ForceComponent : public ska::Component {
	public:
		float x;
		float y;
		float z;
	};
}
