#pragma once
#include "../ECS/Component.h"
namespace ska {
	class GravityAffectedComponent : public ska::Component {
	public:
		float friction;
		float weight;
	};
}
