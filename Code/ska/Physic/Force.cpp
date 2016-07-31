#include "Force.h"

ska::Force::Force(ska::Rectangle pos, float power, float angle)
{
	m_pos = pos;
	m_power = power;
	m_angle = angle;
}

ska::Force::Force()
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

ska::Rectangle ska::Force::getPos()
{
	return m_pos;
}

float ska::Force::getPower() const
{
	return m_power;
}

float ska::Force::getAngle() const
{
	return m_angle;
}

void ska::Force::setPos(ska::Rectangle pos)
{
	m_pos = pos;
}

void ska::Force::setPower(float power)
{
	m_power = power;
}

void ska::Force::setAngle(float angle)
{
	m_angle = angle;
}

ska::Force::~Force(){}

