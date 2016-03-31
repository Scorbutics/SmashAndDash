#pragma once
#include "../ECS/Component.h"
namespace ska {
	class InputComponent : public ska::Component {
	public:
		unsigned int movePower;
		unsigned int jumpPower;
	};
}
