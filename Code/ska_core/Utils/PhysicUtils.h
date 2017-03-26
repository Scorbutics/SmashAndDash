#pragma once

#include "Force.h"
namespace ska {
	class PhysicUtils
	{
	private:
		PhysicUtils();
	public:
		static const Force getMovement(const int direction, const float speed);
		~PhysicUtils();
	};
}
