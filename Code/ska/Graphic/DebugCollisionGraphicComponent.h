#pragma once
#include <queue>
#include "../ECS/Component.h"
#include "../ECS/ECSDefines.h"

namespace ska {
	class DebugCollisionGraphicComponent : public Component {
	public:
		std::queue<ska::EntityId> collidedBlocks;
	};
}