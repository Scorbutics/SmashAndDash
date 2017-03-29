#pragma once
#include <queue>
#include "../../Component.h"
#include "../../ECSDefines.h"

namespace ska {
	enum DebugGraphicType {
		COLLISION = (1u << 0),
		WALK = (1u << 1)
	};
	class DebugGraphicComponent : public Component {
	public:
		DebugGraphicComponent();
		virtual ~DebugGraphicComponent() = default;
		unsigned int typeMask;
		std::queue<EntityId> collidedBlocks;
	};
}
