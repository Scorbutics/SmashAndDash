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

void ska::CameraSystem::focusOn(Point<int>& pos, EntityId* entityId) {
	m_cameraRect.x = pos.x - m_cameraRect.w / 2;
	m_cameraRect.y = pos.y - m_cameraRect.h / 2;

	if (entityId != NULL && m_entityManager.hasComponent<GraphicComponent>(*entityId)) {
		GraphicComponent& gc = m_entityManager.getComponent<GraphicComponent>(*entityId);
		if (!gc.sprite.empty()) {
			ska::AnimatedTexture& texture = gc.sprite[0];

			m_cameraRect.x += texture.getWidth() / 2;
			m_cameraRect.y += texture.getHeight() / 2;
		}
	}

	if (m_cameraRect.x < 0) {
		m_cameraRect.x = 0;
	}
	else if (m_cameraRect.x + m_cameraRect.w > m_worldW) {
		m_cameraRect.x = m_worldW - m_cameraRect.w;
	}

	if (m_cameraRect.y < 0) {
		m_cameraRect.y = 0;
	}
	else if (m_cameraRect.y + m_cameraRect.h > m_worldH) {
		m_cameraRect.y = m_worldH - m_cameraRect.h;
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
