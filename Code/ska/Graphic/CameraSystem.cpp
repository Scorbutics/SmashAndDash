#include "CameraSystem.h"
#include "Rectangle.h"

ska::CameraSystem::CameraSystem(EntityManager& entityManager, const unsigned int screenW, const unsigned int screenH) : System(entityManager) {
	m_pos = NULL;
	screenResized(screenW, screenH);
}

void ska::CameraSystem::screenResized(const unsigned int screenW, const unsigned int screenH) {
	m_cameraRect.w = screenW;
	m_cameraRect.h = screenH;
}

void ska::CameraSystem::refresh(EntityId& entity) {
	m_pos = &m_entityManager.getComponent<PositionComponent>(entity);
	m_cameraRect.x = m_pos->x;
	m_cameraRect.y = m_pos->y;
}

const ska::Rectangle* ska::CameraSystem::getDisplay() const {
	if (m_pos == NULL) {
		return NULL;
	}

	return &m_cameraRect;
}

ska::CameraSystem::~CameraSystem()
{
}
