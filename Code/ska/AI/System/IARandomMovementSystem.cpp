#include "IARandomMovementSystem.h"
#include "../../Utils/TimeUtils.h"
#include "../../Utils/PhysicUtils.h"
#include "../../Utils/NumberUtils.h"

ska::IARandomMovementSystem::IARandomMovementSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void ska::IARandomMovementSystem::refresh() {
	for (EntityId entityId : m_processed) {
		ForceComponent& fc = m_entityManager.getComponent<ForceComponent>(entityId);
		IARandomMovementComponent& iamc = m_entityManager.getComponent<IARandomMovementComponent>(entityId);

		if (TimeUtils::getTicks() - iamc.lastTimeStarted >= iamc.delay) {
			if (NumberUtils::random(0, 4) == 0) {
				iamc.direction = NumberUtils::random(0, 7);
			} else {
				iamc.direction = -1;
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

ska::IARandomMovementSystem::~IARandomMovementSystem() {
}
