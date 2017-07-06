#include "DeleterSystem.h"
#include "Utils/SpritePath.h"

ska::DeleterSystem::DeleterSystem(EntityManager& entityManager) : System(entityManager) {
}

void ska::DeleterSystem::refresh(unsigned int) {
	std::vector<EntityId> toDelete;
	for (const auto& entityId : m_processed) {
		auto& dc = m_entityManager.getComponent<DeleterComponent>(entityId);
		const int elapsed = TimeUtils::getTicks() - dc.startTime;
		if (elapsed >= dc.delay) {
			toDelete.push_back(entityId);
		}
	}

	for (const auto& entityId : toDelete) {
		m_entityManager.removeEntity(entityId);
	}
}

ska::DeleterSystem::~DeleterSystem() {
}
