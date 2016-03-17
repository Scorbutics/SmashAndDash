#include "CameraSystem.h"
#include "../Rectangle.h"

ska::CameraSystem::CameraSystem(EntityManager& entityManager, const unsigned int screenW, const unsigned int screenH) : System(entityManager) {
	m_pos = NULL;
	screenResized(screenW, screenH);
}

void ska::CameraSystem::screenResized(const unsigned int screenW, const unsigned int screenH) {
	m_cameraRect.w = screenW;
	m_cameraRect.h = screenH;
}

void ska::CameraSystem::refresh() {
	for (ska::EntityId entityId : m_processed) {
		m_pos = &m_entityManager.getComponent<PositionComponent>(entityId);
		m_cameraRect.x = m_pos->x - m_cameraRect.w / 2;
		m_cameraRect.y = m_pos->y - m_cameraRect.h / 2;
	}
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
