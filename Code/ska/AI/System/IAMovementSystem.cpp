#include "IAMovementSystem.h"
#include "../../Utils/TimeUtils.h"
#include "../../Utils/PhysicUtils.h"
#include "../../Utils/NumberUtils.h"

ska::IAMovementSystem::IAMovementSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void ska::IAMovementSystem::refresh() {
	for (EntityId entityId : m_processed) {
		ForceComponent& fc = m_entityManager.getComponent<ForceComponent>(entityId);
		IAMovementComponent& iamc = m_entityManager.getComponent<IAMovementComponent>(entityId);

		if (TimeUtils::getTicks() - iamc.lastTimeStarted >= iamc.delay) {
			
			switch (iamc.type) {
			case 0:
				if (++iamc.currentPathIndex >= iamc.path.size()) {
					iamc.currentPathIndex = 0;
				}
				iamc.direction = iamc.path[iamc.currentPathIndex];
				break;
			
			case 1:
				if (NumberUtils::random(0, 4) == 0) {
					iamc.direction = NumberUtils::random(0, 7);
				} else {
					iamc.direction = -1;
				}
				break;
			
			default:
				break;
			}

			iamc.lastTimeStarted = TimeUtils::getTicks();
		}

		if (iamc.direction != -1) {
			ska::Force f = PhysicUtils::getMovement(iamc.direction, 100);
			fc.x += f.getPower() * NumberUtils::cosinus(f.getAngle());
			fc.y += f.getPower() * NumberUtils::sinus(f.getAngle());
		}
	}
}

ska::IAMovementSystem::~IAMovementSystem() {
}
