#include <iostream>
#include "CameraFollowSystem.h"
#include "Rectangle.h"
#include "../GraphicComponent.h"

ska::CameraFollowSystem::CameraFollowSystem(EntityManager& entityManager, const unsigned int screenW, const unsigned int screenH) : CameraSystem(entityManager, screenW, screenH) {
}

void ska::CameraFollowSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	auto mainEntityIt = processed.begin();
	if(mainEntityIt != processed.end()) {
		auto entityId = *mainEntityIt;
		auto& pc = m_componentAccessor.get<PositionComponent>(entityId);
		m_follow = { pc.x, pc.y, 0, 0 };
		m_pos = &m_follow;
		focusOn(m_follow, &entityId);
	}

}

ska::CameraFollowSystem::~CameraFollowSystem()
{
}
