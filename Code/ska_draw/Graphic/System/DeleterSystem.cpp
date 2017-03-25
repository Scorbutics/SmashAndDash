#include "DeleterSystem.h"
#include "Utils/SpritePath.h"

ska::DeleterSystem::DeleterSystem(EntityManager& entityManager) : System(entityManager) {
}

void ska::DeleterSystem::refresh() {
	std::vector<EntityId> toDelete;
	for (EntityId entityId : m_processed) {
		DeleterComponent& dc = m_entityManager.getComponent<DeleterComponent>(entityId);
		const int elapsed = TimeUtils::getTicks() - dc.startTime;
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
