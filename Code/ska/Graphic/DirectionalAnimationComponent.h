#pragma once
#include "../ECS/Component.h"
namespace ska {
	class DirectionalAnimationComponent : public Component {
	public:
		int direction;
	};
}