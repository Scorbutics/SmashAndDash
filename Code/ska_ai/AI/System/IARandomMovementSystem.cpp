#include "IARandomMovementSystem.h"
#include "Utils/TimeUtils.h"
#include "Utils/PhysicUtils.h"
#include "Utils/NumberUtils.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "Utils/RectangleUtils.h"

ska::IARandomMovementSystem::IARandomMovementSystem(EntityManager& entityManager) : System(entityManager) {
}

void ska::IARandomMovementSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	for (auto entityId : processed) {
		auto& fc = m_componentAccessor.get<ForceComponent>(entityId);
		auto& iamc = m_componentAccessor.get<IARandomMovementComponent>(entityId);

		if (TimeUtils::getTicks() - iamc.lastTimeStarted >= iamc.delay) {

			if (NumberUtils::random(0, 4) != 0) {
				iamc.direction = -1;
			} else {

				switch (iamc.type) {
				case RandomMovementType::NO_GOAL:
					iamc.direction = static_cast<char>(NumberUtils::random(0, 7));
					break;
				case RandomMovementType::CIRCLE_AROUND: {
                        char sens = NumberUtils::random(0, 1) == 0;
                        /* It has no sense to have a IARandomMovementComponent attached to an entity that has no PositionComponent, so we assume we can get it */
                        auto& pc = *m_componentPossibleAccessor.get<PositionComponent>(entityId);
                        /* But we check the emitter */
						const auto& pcEmitterPtr = m_componentPossibleAccessor.get<PositionComponent>(iamc.emitter);
                        if (pcEmitterPtr != nullptr) {
                            auto& pcEmitter = *pcEmitterPtr;
                            auto diffPoint = Point<int>(pcEmitter.x, pcEmitter.y) - Point<int>(pc.x, pc.y);
                            auto horizontalDir = NumberUtils::maximum(NumberUtils::absolute(diffPoint.x), NumberUtils::absolute(diffPoint.y)) == NumberUtils::absolute(diffPoint.x);
                            iamc.direction = horizontalDir ? (sens ? 1 : 3) : (sens ? 0 : 2);

                            /* Change the direction the entity look at */
							const auto& dacPtr = m_componentPossibleAccessor.get<DirectionalAnimationComponent>(entityId);
                            if (dacPtr != nullptr) {
                                auto& dac = *dacPtr;
                                dac.looked = iamc.emitter;
                                dac.type = DirectionalAnimationType::MOVEMENT_THEN_FOLLOWING;
                            }

                        } else {
                            iamc.direction = -1;
                        }
                    }
					break;

                default:
                    break;
				}
			}
			iamc.lastTimeStarted = TimeUtils::getTicks();
		}

		if (iamc.direction != -1) {
			auto f = PhysicUtils::getMovement(iamc.direction, 100);
			fc.x += static_cast<float>(f.power * NumberUtils::cosinus(f.angle));
			fc.y += static_cast<float>(f.power * NumberUtils::sinus(f.angle));
		}
	}
}

ska::IARandomMovementSystem::~IARandomMovementSystem() {
}
