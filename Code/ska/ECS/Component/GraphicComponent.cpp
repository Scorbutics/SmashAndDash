#include "GraphicComponent.h"
#include "PositionComponent.h"

ska::GraphicComponent::GraphicComponent() {
}

ska::Texture& ska::GraphicComponent::getSprite() {
	return m_sprite;
}

ska::GraphicComponent::~GraphicComponent() {
}