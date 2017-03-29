#include "IADefinedMovementSystem.h"
#include "Utils/TimeUtils.h"
#include "Utils/RectangleUtils.h"
#include "ECS/Basics/Script/ScriptRegisterer.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"

ska::IADefinedMovementSystem::IADefinedMovementSystem(EntityManager& entityManager, ScriptRegisterer* scriptSystem) : System(entityManager), m_scriptSystem(scriptSystem) {
}

void ska::IADefinedMovementSystem::refresh() {
	std::vector<EntityId> entityWithComponentsToDelete;

	for (EntityId entityId : m_processed) {
		auto& mc = m_entityManager.getComponent<MovementComponent>(entityId);
		auto& pc = m_entityManager.getComponent<PositionComponent>(entityId);
		auto& iamc = m_entityManager.getComponent<IADefinedMovementComponent>(entityId);
		const auto& hc = m_entityManager.getComponent<HitboxComponent>(entityId);
		const auto& centerPos = PositionComponent::getCenterPosition(pc, hc);

		if (iamc.directionIndex >= iamc.directions.size()) {
			continue;
		}

		const auto& targetVector = iamc.directions[iamc.directionIndex];
		auto polarVectorMovement = PolarPoint<int>::polar(targetVector.x, targetVector.y);

		/* speed */
		polarVectorMovement.radius = 5;

		const auto targetPoint = targetVector + iamc.origin;
		const auto finalMovement = Point<float>::cartesian(static_cast<float>(polarVectorMovement.radius), polarVectorMovement.angle);

		/* Either the time is up, or the goal is reached (if we are going farer and farer from the target pos, goal is reached) */
		const auto distanceSquaredToTarget = RectangleUtils::distanceSquared(centerPos, targetPoint);
		const auto nextDistanceSquaredToTarget = RectangleUtils::distanceSquared(centerPos + finalMovement, targetPoint);
		const auto directionChanged = distanceSquaredToTarget < nextDistanceSquaredToTarget;

		bool collisioned;
		if (iamc.ghost) {
			m_entityManager.removeComponent<WorldCollisionComponent>(entityId);
			collisioned = false;
		} else {
			collisioned = m_entityManager.hasComponent<WorldCollisionComponent>(entityId);
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
					auto scriptEntity = m_entityManager.createEntity();
					m_entityManager.addComponent<ScriptSleepComponent>(scriptEntity, iamc.callback);
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
		m_entityManager.removeComponent<IADefinedMovementComponent>(id);
	}
}

ska::IADefinedMovementSystem::~IADefinedMovementSystem() {
}
