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
		std::vector<ska::Point<int>> blockColPosX;
		std::vector<ska::Point<int>> lastBlockColPosX;
		std::vector<ska::Point<int>> blockColPosY;
		std::vector<ska::Point<int>> lastBlockColPosY;
	};
}