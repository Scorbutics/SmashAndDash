#pragma once
#include "../ECS/Component.h"
namespace ska {
	class GravityAffectedComponent : public Component {
	public:
		float friction;
	};
}
