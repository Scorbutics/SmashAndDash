#pragma once
#include "../../Component.h"
namespace ska {
	class InputComponent : public Component {
	public:
	    InputComponent();
	    ~InputComponent() = default;
		float movePower;
		float jumpPower;
	};
}
