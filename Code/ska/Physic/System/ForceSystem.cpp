#include "ForceSystem.h"

ska::ForceSystem::ForceSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void ska::ForceSystem::refresh() {
	for (ska::EntityId entityId : m_processed) {
		ForceComponent& forceComponent = m_entityManager.getComponent<ForceComponent>(entityId);
		MovementComponent& moveComponent = m_entityManager.getComponent<MovementComponent>(entityId);

		/* sum(F) = m*a */
		moveComponent.ax = forceComponent.x / forceComponent.weight;
		moveComponent.ay = forceComponent.y / forceComponent.weight;
		moveComponent.az = forceComponent.z / forceComponent.weight;
		
		/* Don't forget to reset the current forces applied to */
		forceComponent.x = 0;
		forceComponent.y = 0;
		forceComponent.z = 0;
	}
}

ska::ForceSystem::~ForceSystem()
{
}
