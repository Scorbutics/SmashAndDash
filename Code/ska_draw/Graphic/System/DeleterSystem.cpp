#include "DeleterSystem.h"
#include "Utils/SpritePath.h"

ska::DeleterSystem::DeleterSystem(EntityManager& entityManager) : System(entityManager) {
}

void ska::DeleterSystem::refresh(unsigned int) {
	std::vector<EntityId> toDelete;
	const auto& processed = getEntities();
	for (const auto& entityId : processed) {
		auto& dc = m_componentAccessor.get<DeleterComponent>(entityId);
		const int elapsed = TimeUtils::getTicks() - dc.startTime;
		if (elapsed >= dc.delay) {
			toDelete.push_back(entityId);
		}
	}

	for (const auto& entityId : toDelete) {
		removeEntity(entityId);
	}
}

ska::DeleterSystem::~DeleterSystem() {
}
