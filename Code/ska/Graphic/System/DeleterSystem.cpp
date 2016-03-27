#include "DeleterSystem.h"
#include "../SpritePath.h"

ska::DeleterSystem::DeleterSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void ska::DeleterSystem::refresh() {
	std::vector<EntityId> toDelete;
	for (ska::EntityId entityId : m_processed) {
		DeleterComponent& dc = m_entityManager.getComponent<DeleterComponent>(entityId);
		const int elapsed = ska::TimeUtils::getTicks() - dc.startTime;
		if (elapsed >= dc.delay) {
			toDelete.push_back(entityId);
		}
	}

	for (EntityId entityId : toDelete) {
		m_entityManager.removeEntity(entityId);
	}
}

ska::DeleterSystem::~DeleterSystem() {
}
