#pragma once
#include "../../Component.h"
#include "../Physic/PositionComponent.h"

namespace ska {
	class ScriptAwareComponent : public Component {
	public:
		PositionComponent lastBlockPos;
	};
}