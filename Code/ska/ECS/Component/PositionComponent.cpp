#include "PositionComponent.h"


ska::PositionComponent::PositionComponent()
{
}

const int ska::PositionComponent::getX() const {
	return m_x;
}

const int ska::PositionComponent::getY() const {
	return m_y;
}

const int ska::PositionComponent::getZ() const {
	return m_z;
}

void ska::PositionComponent::setX(const int x) {
	m_x = x;
}

void ska::PositionComponent::setY(const int y) {
	m_y = y;
}

void ska::PositionComponent::setZ(const int z) {
	m_z = z;
}

ska::PositionComponent::~PositionComponent()
{
}
