#include "../../ska/Graphic/System/CameraSystem.h"
#include "AbstractGraphicSystem.h"
#include "../../ska/Graphic/Draw/DrawableContainer.h"
#include "../../ska/Graphic/PositionnedGraphicDrawable.h"

AbstractGraphicSystem::AbstractGraphicSystem(ska::CameraSystem* camera) : m_camera(camera) {
	m_drawables = nullptr;
}

void AbstractGraphicSystem::linkCamera(ska::CameraSystem* camera) {
	m_camera = camera;
}

void AbstractGraphicSystem::setDrawables(ska::DrawableContainer& container) {
	m_drawables = &container;
}

AbstractGraphicSystem::~AbstractGraphicSystem() {
}
