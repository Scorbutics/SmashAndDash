#pragma once
#include "../../Component.h"
#include "../../ECSDefines.h"
namespace ska {
	class CollisionComponent : public Component {
	public:
		CollisionComponent();
		EntityId origin;
		EntityId target;
		bool xaxis;
		bool yaxis;
	};
}
