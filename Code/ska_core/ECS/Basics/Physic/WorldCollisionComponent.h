#pragma once
#include "../../ska/ECS/Component.h"
#include "../../ska/ECS/ECSDefines.h"
#include "../Graphic/Point.h"
namespace ska {
	class WorldCollisionComponent : public Component {
	public:
		WorldCollisionComponent() { xaxis = false; yaxis = false; }
		bool xaxis;
		bool yaxis;
		std::vector<Point<int>> blockColPosX;
		std::vector<Point<int>> lastBlockColPosX;
		std::vector<Point<int>> blockColPosY;
		std::vector<Point<int>> lastBlockColPosY;
	};
}
