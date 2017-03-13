#include <cmath>
#include "PhysicUtils.h"


ska::PhysicUtils::PhysicUtils()
{
}

const ska::Force ska::PhysicUtils::getMovement(const int direction, const float speed) {
	float angle;
	Force f;

	switch (direction)
	{
	case 0:
		angle = (float)M_PI / 2;
		break;

	case 1:
		angle = (float)0.;
		break;

	case 2:
		angle = -(float)M_PI / 2;
		break;

	case 3:
		angle = -(float)M_PI;
		break;

	case 4:
		angle = (float)M_PI / 4;
		break;

	case 5:
		angle = -(float)M_PI / 4;
		break;

	case 6:
		angle = (float)(-3 * M_PI / 4);
		break;

	case 7:
		angle = (float)(3 * M_PI / 4);
		break;

	default:
		angle = 0;
		break;
	}

	f.angle = (angle);
	f.power = (speed);
	return f;
}

ska::PhysicUtils::~PhysicUtils()
{
}
