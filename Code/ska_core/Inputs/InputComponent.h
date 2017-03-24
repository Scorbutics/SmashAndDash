#pragma once
#include "../ECS/Component.h"
namespace ska {
	class InputComponent : public Component {
	public:
		unsigned int movePower;
		unsigned int jumpPower;
	};
}
