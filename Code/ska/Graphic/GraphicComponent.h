#pragma once
#include "../ECS/Component.h"
#include "AnimatedTexture.h"

namespace ska {
	class GraphicComponent : public ska::Component {
	public:
		GraphicComponent() {
			desiredPriority = INT_MIN;
		}

		int desiredPriority;
		std::vector<ska::AnimatedTexture> sprite;
		void operator=(const GraphicComponent& gc) {
			sprite = gc.sprite;
			desiredPriority = gc.desiredPriority;
		}

	};
}