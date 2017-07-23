#include "CameraSystem.h"
#include "Rectangle.h"
#include "../GraphicComponent.h"

ska::CameraSystem::CameraSystem(EntityManager& entityManager, const unsigned int screenW, const unsigned int screenH) :
    System(entityManager),
    m_pos(nullptr) {

	worldResized(screenW, screenH);
	screenResized(screenW, screenH);
}

void ska::CameraSystem::worldResized(const unsigned int worldW, const unsigned int worldH) {
	m_worldW = worldW;
	m_worldH = worldH;
}

ska::Point<int> ska::CameraSystem::getScreenSize() const {
	return ska::Point<int>(m_cameraRect.w, m_cameraRect.h);
}

void ska::CameraSystem::screenResized(const unsigned int screenW, const unsigned int screenH) {
	m_cameraRect.w = screenW;
	m_cameraRect.h = screenH;
}

void ska::CameraSystem::focusOn(Rectangle& pos, EntityId* entityId) {
	m_cameraRect.x = pos.x - m_cameraRect.w / 2;
	m_cameraRect.y = pos.y - m_cameraRect.h / 2;

	const auto& graphicComponentPtr = entityId == nullptr ? nullptr : m_componentPossibleAccessor.get<GraphicComponent>(*entityId);
	if (entityId != nullptr && graphicComponentPtr != nullptr) {
		auto& gc = *graphicComponentPtr;
		if (!gc.sprite.empty()) {
			auto& texture = gc.sprite[0];
			m_cameraRect.x += texture.getWidth() / 2;
			m_cameraRect.y += texture.getHeight() / 2;
		}
	}

	if (m_cameraRect.x < 0) {
		m_cameraRect.x = 0;
	} else if (static_cast<unsigned int>(m_cameraRect.x + m_cameraRect.w) > m_worldW) {
		m_cameraRect.x = m_worldW - m_cameraRect.w;
	}

	if (m_cameraRect.y < 0) {
		m_cameraRect.y = 0;
	} else if (static_cast<unsigned int>(m_cameraRect.y + m_cameraRect.h) > m_worldH) {
		m_cameraRect.y = m_worldH - m_cameraRect.h;
	}
}


const ska::Rectangle* ska::CameraSystem::getDisplay() const {
	if (m_pos == nullptr) {
		return nullptr;
	}

	return &m_cameraRect;
}

ska::CameraSystem::~CameraSystem()
{
}
