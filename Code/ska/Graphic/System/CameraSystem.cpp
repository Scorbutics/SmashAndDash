#include "CameraSystem.h"
#include "../Rectangle.h"
#include "../GraphicComponent.h"

ska::CameraSystem::CameraSystem(EntityManager& entityManager, const unsigned int screenW, const unsigned int screenH) : System(entityManager) {
	m_pos = NULL;
	worldResized(screenW, screenH);
	screenResized(screenW, screenH);
}

void ska::CameraSystem::worldResized(const unsigned int worldW, const unsigned int worldH) {
	m_worldW = worldW;
	m_worldH = worldH;
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

		if (m_entityManager.hasComponent<GraphicComponent>(entityId)) {
			GraphicComponent& gc = m_entityManager.getComponent<GraphicComponent>(entityId);
			m_cameraRect.x += gc.sprite.getWidth() / 2;
			m_cameraRect.y += gc.sprite.getHeight() / 2;
		}
		
		if (m_cameraRect.x < 0) {
			m_cameraRect.x = 0;
		} else if (m_cameraRect.x + m_cameraRect.w > m_worldW) {
			m_cameraRect.x = m_worldW - m_cameraRect.w;
		}

		if (m_cameraRect.y < 0) {
			m_cameraRect.y = 0;
		} else if (m_cameraRect.y + m_cameraRect.h > m_worldH) {
			m_cameraRect.y = m_worldH - m_cameraRect.h;
		}

		
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