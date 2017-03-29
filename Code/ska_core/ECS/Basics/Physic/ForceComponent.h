#pragma once
#include "../../Component.h"

namespace ska {
	class ForceComponent : public Component{
	public:
		ForceComponent();
		~ForceComponent() = default;

		float x;
		float y;
		float z;

		float weight;
	};
}
