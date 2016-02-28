#include "Force.h"

using namespace std;

Force::Force(ska::Rectangle pos, float power, float angle)
{
	m_pos = pos;
	m_power = power;
	m_angle = angle;
}

Force::Force()
{
	ska::Rectangle nullPos;
	nullPos.x = 0;
	nullPos.y = 0;
	nullPos.w = 0;
	nullPos.h = 0;
    m_pos = nullPos;
    m_power = 0;
    m_angle = 0.;
}

ska::Rectangle Force::getPos()
{
	return m_pos;
}

float Force::getPower()
{
	return m_power;
}

float Force::getAngle()
{
	return m_angle;
}

void Force::setPos(ska::Rectangle pos)
{
	m_pos = pos;
}

void Force::setPower(float power)
{
	m_power = power;
}

void Force::setAngle(float angle)
{
	m_angle = angle;
}

Force::~Force(){}

