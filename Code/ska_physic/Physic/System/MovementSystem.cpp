#include "MovementSystem.h"
#include "Utils/NumberUtils.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "Ticked.h"

ska::MovementSystem::MovementSystem(ska::EntityManager& entityManager, Ticked& ticked) :
	ska::System<std::unordered_set<EntityId>, PositionComponent, MovementComponent, ForceComponent> (entityManager), 
	m_ticks(ticked.ticksWanted()) {
	name("MovementSystem");
}

void ska::MovementSystem::refresh(unsigned int ellapsedTime) {
	for (auto entityId : m_processed) {
		auto& posComponent = m_entityManager.getComponent<PositionComponent>(entityId);
		auto& moveComponent = m_entityManager.getComponent<MovementComponent>(entityId);
		auto& forceComponent = m_entityManager.getComponent<ForceComponent>(entityId);

		if (m_entityManager.hasComponent<WorldCollisionComponent>(entityId)) {
			auto& col = m_entityManager.getComponent<WorldCollisionComponent>(entityId);

			/* World Collision */
			if (col.xaxis) {
				moveComponent.vx = 0;
				moveComponent.ax = 0;
			}

			if (col.yaxis) {
				moveComponent.vy = 0;
				moveComponent.ay = 0;
			}

		}

		//Position vector is the current position of the object (i.e. the last position + the last velocity with a delta time taken)
		//(x(t) - x(t-1))/(t - (t-1)) = dx/dt (t) = vx(t)
		posComponent.x += static_cast<int>(moveComponent.vx);
		posComponent.y += static_cast<int>(moveComponent.vy);
		posComponent.z += static_cast<int>(moveComponent.vz);

		//Velocity vector ALWAYS means the NEXT velocity and not the current one
		//(v(t) - v(t-1))/(t - (t-1)) = dv/dt (t) = a(t)
		moveComponent.vx += moveComponent.ax;
		moveComponent.vy += moveComponent.ay;
		moveComponent.vz += moveComponent.az;

		//Acceleration vector ALWAYS means the next acceleration, and not the current one
		/* sum(F) = m*a */
		moveComponent.ax = (forceComponent.x / forceComponent.weight);
		moveComponent.ay = (forceComponent.y / forceComponent.weight);
		moveComponent.az = (forceComponent.z / forceComponent.weight);

		/* (Don't forget to reset the current forces applied to, because once forces are used, they do) */
		forceComponent.x = 0;
		forceComponent.y = 0;
		forceComponent.z = 0;

		//The acceleration, velocity and position order is important, as they are used in other systems. 
		//So velocity and acceleration have to ALWAYS BE the NEXT one.

		/* Ground reflection */
		if (posComponent.z < 0 || (posComponent.z + moveComponent.vz) <= 0) {
			posComponent.z = 0;
			moveComponent.vz = 0;
		}

		/* Ignore low values */
		if (NumberUtils::absolute(moveComponent.vx) < 0.001) {
			moveComponent.vx = 0;
		}

		if (NumberUtils::absolute(moveComponent.vy) < 0.001) {
			moveComponent.vy = 0;
		}

		if (NumberUtils::absolute(moveComponent.vz) < 0.001) {
			moveComponent.vz = 0;
		}
	}
}

ska::MovementSystem::~MovementSystem()
{
}
