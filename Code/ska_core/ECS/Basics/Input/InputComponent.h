#pragma once
#include "../../Component.h"
namespace ska {
	class InputComponent : public Component {
	public:
	    InputComponent();
	    ~InputComponent() = default;
		unsigned int movePower;
		unsigned int jumpPower;
	};
}
