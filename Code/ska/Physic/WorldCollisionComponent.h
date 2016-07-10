#pragma once
#include "../../ska/ECS/Component.h"
#include "../../ska/ECS/ECSDefines.h"
#include "../Graphic/Point.h"
namespace ska {
	class WorldCollisionComponent : public ska::Component {
	public:
		WorldCollisionComponent() { xaxis = false; yaxis = false; }
		bool xaxis;
		bool yaxis;
		ska::Point<int> blockColPosX;
		ska::Point<int> lastBlockColPosX;
		ska::Point<int> blockColPosY;
		ska::Point<int> lastBlockColPosY;
	};
}