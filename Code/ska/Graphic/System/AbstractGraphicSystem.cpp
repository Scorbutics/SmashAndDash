#include "AbstractGraphicSystem.h"
#include "../PositionnedGraphicDrawable.h"

ska::AbstractGraphicSystem::AbstractGraphicSystem(ska::CameraSystem& camera) : m_camera(camera) {
	m_drawables = NULL;
}

void ska::AbstractGraphicSystem::setDrawables(DrawableContainer& container) {
	m_drawables = &container;
}

ska::AbstractGraphicSystem::~AbstractGraphicSystem() {
}
