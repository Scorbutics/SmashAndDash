#include "ForceSystem.h"

ska::ForceSystem::ForceSystem(EntityManager& entityManager) : System(entityManager) {
	name("ForceSystem");
}

void ska::ForceSystem::refresh(unsigned int ellapsedTime) {
	for (auto entityId : m_processed) {
/*		auto& forceComponent = m_entityManager.getComponent<ForceComponent>(entityId);
		auto& moveComponent = m_entityManager.getComponent<MovementComponent>(entityId);

		/* sum(F) = m*a #1#
		moveComponent.ax = forceComponent.x / forceComponent.weight;
		moveComponent.ay = forceComponent.y / forceComponent.weight;
		moveComponent.az = forceComponent.z / forceComponent.weight;
		
		/* Don't forget to reset the current forces applied to #1#
		forceComponent.x = 0;
		forceComponent.y = 0;
		forceComponent.z = 0;*/
	}
}

ska::ForceSystem::~ForceSystem()
{
}
