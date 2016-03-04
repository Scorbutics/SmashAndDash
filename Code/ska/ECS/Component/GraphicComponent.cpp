#include "GraphicComponent.h"
#include "PositionComponent.h"

ska::GraphicComponent::GraphicComponent() : m_pos(NULL) {
}

ska::GraphicComponent::GraphicComponent(const PositionComponent* pos) : m_pos(pos) {
}

const ska::PositionComponent* ska::GraphicComponent::getPositionComponent() const {
	return m_pos;
}

ska::Texture& ska::GraphicComponent::getSprite() {
	return m_sprite;
}

ska::GraphicComponent::~GraphicComponent() {
}