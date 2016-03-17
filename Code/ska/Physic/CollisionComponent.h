#pragma once
#include "../../ska/ECS/Component.h"
#include "../../ska/ECS/ECSDefines.h"
namespace ska {
	class CollisionComponent : public ska::Component {
	public:
		ska::EntityId origin;
		ska::EntityId target;
		bool world;
	};
}