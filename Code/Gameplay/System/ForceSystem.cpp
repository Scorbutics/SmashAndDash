#include "ForceSystem.h"

ForceSystem::ForceSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void ForceSystem::refresh(ska::EntityId& entityId) {
	ForceComponent& forceComponent = m_entityManager.getComponent<ForceComponent>(entityId);
	MovementComponent& moveComponent = m_entityManager.getComponent<MovementComponent>(entityId);

	moveComponent.ax += forceComponent.x;
	moveComponent.ay += forceComponent.y;
	moveComponent.az += forceComponent.z;

}

ForceSystem::~ForceSystem()
{
}
