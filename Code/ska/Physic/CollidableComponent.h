#pragma once
#include "../../ska/ECS/Component.h"

namespace ska {
	class CollidableComponent : public ska::Component {
	public:
		CollidableComponent() = default;
		~CollidableComponent() = default;
	};
}