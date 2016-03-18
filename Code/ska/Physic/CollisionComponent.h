#pragma once
#include "../../ska/ECS/Component.h"
#include "../../ska/ECS/ECSDefines.h"
namespace ska {
	class CollisionComponent : public ska::Component {
	public:
		CollisionComponent() { xaxis = false; yaxis = false; origin = 0; target = 0; world = false; entities = false; }
		ska::EntityId origin;
		ska::EntityId target;
		bool xaxis;
		bool yaxis;
		bool world;
		bool entities;
	};
}