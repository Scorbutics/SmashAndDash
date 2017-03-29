#pragma once
#include "ECS/Component.h"

namespace ska {
	class HasShadowComponent : public Component {
	public:
	    HasShadowComponent();
	    ~HasShadowComponent() = default;
	};
}
