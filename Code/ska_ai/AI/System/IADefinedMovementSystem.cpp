#include "IADefinedMovementSystem.h"
#include "Utils/TimeUtils.h"
#include "Utils/RectangleUtils.h"
#include "ECS/Basics/Script/ScriptRegisterer.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"

ska::IADefinedMovementSystem::IADefinedMovementSystem(EntityManager& entityManager, ScriptRegisterer* scriptSystem) : System(entityManager), m_scriptSystem(scriptSystem) {
}

void ska::IADefinedMovementSystem::refresh(unsigned int) {
	std::vector<EntityId> entityWithComponentsToDelete;

	const auto& processed = getEntities();
	for (EntityId entityId : processed) {
		auto& mc = m_componentAccessor.get<MovementComponent>(entityId);
		auto& pc = m_componentAccessor.get<PositionComponent>(entityId);
		auto& iamc = m_componentAccessor.get<IADefinedMovementComponent>(entityId);
		const auto& hc = m_componentAccessor.get<HitboxComponent>(entityId);
		const auto& centerPos = PositionComponent::getCenterPosition(pc, hc);

		if (iamc.directionIndex >= iamc.directions.size()) {
			continue;
		}

		const auto& targetVector = iamc.directions[iamc.directionIndex];
		auto polarVectorMovement = PolarPoint<float>::polar(targetVector.x, targetVector.y);

		/* speed */
		polarVectorMovement.radius = 5.F;

		const auto targetPoint = targetVector + iamc.origin;
		const auto finalMovement = Point<float>::cartesian(polarVectorMovement.radius, polarVectorMovement.angle);

		/* Either the time is up, or the goal is reached (if we are going farer and farer from the target pos, goal is reached) */
		const auto distanceSquaredToTarget = RectangleUtils::distanceSquared(centerPos, targetPoint);
		const auto nextDistanceSquaredToTarget = RectangleUtils::distanceSquared(centerPos + finalMovement, targetPoint);
		const auto directionChanged = distanceSquaredToTarget < nextDistanceSquaredToTarget;

		bool collisioned;
		if (iamc.ghost) {
			m_componentAccessor.remove<WorldCollisionComponent>(entityId);
			collisioned = false;
		} else {
			collisioned = m_componentPossibleAccessor.get<WorldCollisionComponent>(entityId) != nullptr;
		}
		auto finished = false;
		if (TimeUtils::getTicks() - iamc.lastTimeStarted >= iamc.delay || directionChanged || collisioned) {

			iamc.origin = iamc.directions[iamc.directionIndex];
			if (iamc.directionIndex+1 < iamc.directions.size()) {
				iamc.directionIndex++;
			} else if (iamc.loop) {
				iamc.directionIndex = 0;
			} else {
				finished = true;
				if (m_scriptSystem != nullptr && iamc.callbackActive) {
					//TODO Event pour remplacer ça
					/* triggers callback */
					auto scriptEntity = createEntity();
					m_componentAccessor.add<ScriptSleepComponent>(scriptEntity, iamc.callback);
					m_scriptSystem->registerScript(nullptr, scriptEntity, entityId);
				}
				/* removes component */
				entityWithComponentsToDelete.push_back(entityId);
			}

			iamc.lastTimeStarted = TimeUtils::getTicks();
		}

		if (!finished) {
			mc.vx = finalMovement.x;
			mc.vy = finalMovement.y;
		} else if (directionChanged) {
			mc.vx = 0;
			mc.vy = 0;
			mc.ay = 0;
			mc.ax = 0;
		}
	}

	for (EntityId id : entityWithComponentsToDelete) {
		m_componentAccessor.remove<IADefinedMovementComponent>(id);
	}
}

ska::IADefinedMovementSystem::~IADefinedMovementSystem() {
}
