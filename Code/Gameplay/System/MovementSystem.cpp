#include "MovementSystem.h"
#include "../Component/CollisionComponent.h"
MovementSystem::MovementSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void MovementSystem::refresh(ska::EntityId& entityId) {
	ska::PositionComponent& posComponent = m_entityManager.getComponent<ska::PositionComponent>(entityId);
	MovementComponent& moveComponent = m_entityManager.getComponent<MovementComponent>(entityId);

	if (m_entityManager.hasComponent<CollisionComponent>(entityId)) {
		CollisionComponent& col = m_entityManager.getComponent<CollisionComponent>(entityId);

		posComponent.x += moveComponent.vx;
		posComponent.y += moveComponent.vy;
		posComponent.z += moveComponent.vz;

		if (!col.world) {
			/* Entity Collision => Apply entity force */
		} else {
			/* World Collision => Rollback the position */
		}

		m_entityManager.removeComponent<CollisionComponent>(entityId);
	}

	//(v(t) - v(t-1))/(t - (t-1)) = dv/dt (t) = a(t)
	moveComponent.vx += moveComponent.ax;
	moveComponent.vy += moveComponent.ay;
	moveComponent.vz += moveComponent.az;

	moveComponent.ax = 0;
	moveComponent.ay = 0;
	moveComponent.az = 0;
	
	//(x(t) - x(t-1))/(t - (t-1)) = dx/dt (t) = vx(t)
	posComponent.x += moveComponent.vx;
	posComponent.y += moveComponent.vy;
	posComponent.z += moveComponent.vz;
	
}

MovementSystem::~MovementSystem()
{
}
