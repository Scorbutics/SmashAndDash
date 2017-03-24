#include "MovementSystem.h"
#include "../../Utils/NumberUtils.h"
#include "../WorldCollisionComponent.h"

ska::MovementSystem::MovementSystem(EntityManager& entityManager) : System(entityManager) {
}

void ska::MovementSystem::refresh() {
	for (auto entityId : m_processed) {
		auto& posComponent = m_entityManager.getComponent<PositionComponent>(entityId);
		auto& moveComponent = m_entityManager.getComponent<MovementComponent>(entityId);

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

		/* Ground reflection */
		if (posComponent.z <= 0 || (posComponent.z + moveComponent.vz) <= 0) {
			posComponent.z = 0;
			moveComponent.vz = 0;
			//moveComponent.az = 0;
		}

		//(v(t) - v(t-1))/(t - (t-1)) = dv/dt (t) = a(t)
		moveComponent.vx += moveComponent.ax;
		moveComponent.vy += moveComponent.ay;
		moveComponent.vz += moveComponent.az;

		/*moveComponent.ax = 0;
		moveComponent.ay = 0;
		moveComponent.az = 0;*/

		//(x(t) - x(t-1))/(t - (t-1)) = dx/dt (t) = vx(t)
		posComponent.x += static_cast<int>(moveComponent.vx);
		posComponent.y += static_cast<int>(moveComponent.vy);
		posComponent.z += static_cast<int>(moveComponent.vz);


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
