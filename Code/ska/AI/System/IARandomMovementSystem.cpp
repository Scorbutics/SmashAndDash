#include "IARandomMovementSystem.h"
#include "../../Utils/TimeUtils.h"
#include "../../Utils/PhysicUtils.h"
#include "../../Utils/NumberUtils.h"
#include "../../Physic/PositionComponent.h"

ska::IARandomMovementSystem::IARandomMovementSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void ska::IARandomMovementSystem::refresh() {
	for (EntityId entityId : m_processed) {
		auto& fc = m_entityManager.getComponent<ForceComponent>(entityId);
		auto& iamc = m_entityManager.getComponent<IARandomMovementComponent>(entityId);

		if (TimeUtils::getTicks() - iamc.lastTimeStarted >= iamc.delay) {

			if (NumberUtils::random(0, 4) != 0) {
				iamc.direction = -1;
			} else {

				switch (iamc.type) {
				case RandomMovementType::NO_GOAL:
					iamc.direction = NumberUtils::random(0, 7);
					break;
				case RandomMovementType::CIRCLE_AROUND:
					char sens = NumberUtils::random(0, 1) == 0;
					/* It has no sense to have a IARandomMovementComponent attached to an entity that has no PositionComponent, so we assume we can get it */
					auto& pc = m_entityManager.getComponent<PositionComponent>(entityId);
					/* But we check the emitter */
					if (m_entityManager.hasComponent<PositionComponent>(iamc.emitter)) {
						auto& pcEmitter = m_entityManager.getComponent<PositionComponent>(iamc.emitter);
						auto diffPoint = ska::Point<int>(pcEmitter.x, pcEmitter.y) - ska::Point<int>(pc.x, pc.y);
						auto horizontalDir = ska::NumberUtils::maximum(ska::NumberUtils::absolute(diffPoint.x), ska::NumberUtils::absolute(diffPoint.y)) == ska::NumberUtils::absolute(diffPoint.x);
						iamc.direction = horizontalDir ? (sens ? 1 : 3) : (sens ? 0 : 2);
					} else {
						iamc.direction = -1;
					}
					break;
				}
			}
			iamc.lastTimeStarted = TimeUtils::getTicks();
		}

		if (iamc.direction != -1) {
			Force f = PhysicUtils::getMovement(iamc.direction, 100);
			fc.x += f.getPower() * NumberUtils::cosinus(f.getAngle());
			fc.y += f.getPower() * NumberUtils::sinus(f.getAngle());
		}
	}
}

ska::IARandomMovementSystem::~IARandomMovementSystem() {
}
