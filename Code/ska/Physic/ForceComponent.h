#pragma once
#include "../ECS/Component.h"

namespace ska {
	class ForceComponent : public Component{
	public:
		ForceComponent() {
			x = 0;
			y = 0;
			z = 0;
			weight = 1;
		}
		float x;
		float y;
		float z;

		float weight;
	};
}
