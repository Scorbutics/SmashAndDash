#pragma once
#include <vector>
#include <limits>
#include "ECS/Component.h"
#include "AnimatedTexture.h"

namespace ska {
	class GraphicComponent : public Component {
	public:
		GraphicComponent();
		void operator=(const GraphicComponent& gc);

		int desiredPriority;
		std::vector<AnimatedTexture> sprite;


	};
}
