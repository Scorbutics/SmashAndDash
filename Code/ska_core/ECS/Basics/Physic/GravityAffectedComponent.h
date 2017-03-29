#pragma once
#include "../../Component.h"
namespace ska {
	class GravityAffectedComponent : public Component {
	public:
	    GravityAffectedComponent();
	    ~GravityAffectedComponent() = default;
		float friction;
	};
}
