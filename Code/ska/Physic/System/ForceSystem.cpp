#include "ForceSystem.h"

ska::ForceSystem::ForceSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void ska::ForceSystem::refresh() {
	for (ska::EntityId entityId : m_processed) {
		ForceComponent& forceComponent = m_entityManager.getComponent<ForceComponent>(entityId);
		MovementComponent& moveComponent = m_entityManager.getComponent<MovementComponent>(entityId);

		moveComponent.ax += forceComponent.x;
		moveComponent.ay += forceComponent.y;
		moveComponent.az += forceComponent.z;
	}
}

ska::ForceSystem::~ForceSystem()
{
}
