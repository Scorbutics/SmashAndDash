#include "CameraFollowSystem.h"
#include "../Rectangle.h"
#include "../GraphicComponent.h"

ska::CameraFollowSystem::CameraFollowSystem(EntityManager& entityManager, const unsigned int screenW, const unsigned int screenH) : CameraSystem(entityManager, screenW, screenH) {
}

void ska::CameraFollowSystem::refresh() {
	for (ska::EntityId entityId : m_processed) {
		PositionComponent& pc = m_entityManager.getComponent<PositionComponent>(entityId);
		m_follow = { pc.x, pc.y, 0, 0 };
		m_pos = &m_follow;
		focusOn(m_follow, &entityId);
		break;
	}
}

ska::CameraFollowSystem::~CameraFollowSystem()
{
}