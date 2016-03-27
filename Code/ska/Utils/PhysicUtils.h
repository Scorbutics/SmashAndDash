#pragma once

#include "../Physic/Force.h"
namespace ska {
	class PhysicUtils
	{
	private:
		PhysicUtils();
	public:
		static const ska::Force getMovement(const int direction, const float speed);
		~PhysicUtils();
	};
}
