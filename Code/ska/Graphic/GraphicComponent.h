#pragma once
#include <vector>
#include <limits>
#include "../ECS/Component.h"
#include "AnimatedTexture.h"

namespace ska {
	class GraphicComponent : public Component {
	public:
		GraphicComponent() {
			desiredPriority = std::numeric_limits<int>().min();
		}

		int desiredPriority;
		std::vector<AnimatedTexture> sprite;
		void operator=(const GraphicComponent& gc) {
			sprite = gc.sprite;
			desiredPriority = gc.desiredPriority;
		}

	};
}
