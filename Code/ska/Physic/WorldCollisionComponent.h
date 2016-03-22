#pragma once
#include "../../ska/ECS/Component.h"
#include "../../ska/ECS/ECSDefines.h"
namespace ska {
	class WorldCollisionComponent : public ska::Component {
	public:
		WorldCollisionComponent() { xaxis = false; yaxis = false; }
		bool xaxis;
		bool yaxis;
	};
}