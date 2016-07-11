#include "CollisionSystem.h"
#include "../../Utils/RectangleUtils.h"
#include "../CollisionComponent.h"
#include "../WorldCollisionComponent.h"

ska::CollisionSystem::CollisionSystem(ska::World& w, ska::EntityManager& entityManager) : System(entityManager), m_world(w) {
}

void ska::CollisionSystem::refresh() {
	for (ska::EntityId entityId : m_processed) {
		PositionComponent& positionComponent = m_entityManager.getComponent<PositionComponent>(entityId);
		HitboxComponent& hitboxComponent = m_entityManager.getComponent<HitboxComponent>(entityId);
		MovementComponent& moveComponent = m_entityManager.getComponent<MovementComponent>(entityId);
		
		ska::Point<int> lastBlockColPosX;
		ska::Point<int> lastBlockColPosY;
		if (m_entityManager.hasComponent<ska::WorldCollisionComponent>(entityId)) {
			const WorldCollisionComponent& wcc = m_entityManager.getComponent<WorldCollisionComponent>(entityId);
			lastBlockColPosX = wcc.blockColPosX;
			lastBlockColPosY = wcc.blockColPosY;
			m_entityManager.removeComponent<WorldCollisionComponent>(entityId);
		}

		const ska::Rectangle entityHitboxX = createHitBox(entityId, true);
		const ska::Rectangle entityHitboxY = createHitBox(entityId, false);
		
		bool entityCollided = false;
		CollisionComponent col;
		for (ska::EntityId itEntity : m_processed) {
			if (itEntity != entityId) {
				if (RectangleUtils::collisionBoxABoxB(entityHitboxX, createHitBox(itEntity, true))) {
					col.origin = entityId;
					col.target = itEntity;
					entityCollided = true;
					col.xaxis = true;
				} 
				if (RectangleUtils::collisionBoxABoxB(entityHitboxY, createHitBox(itEntity, false))) {
					col.origin = entityId;
					col.target = itEntity;
					entityCollided = true;
					col.yaxis = true;
				}

			}

			if (entityCollided) {
				break;
			}

		}

		WorldCollisionComponent wcol;
		bool collided = false;
		Rectangle nextPosX = { entityHitboxX.x, entityHitboxX.y, entityHitboxX.w, entityHitboxX.h };
		Rectangle nextPosY = { entityHitboxY.x, entityHitboxY.y, entityHitboxY.w, entityHitboxY.h };

		ska::Point<int> blockColPosX;
		if (!m_world.canMoveToPos(nextPosX, blockColPosX)){
			collided = true;
			wcol.xaxis = true;
			wcol.lastBlockColPosX = lastBlockColPosX;
			wcol.blockColPosX = blockColPosX * m_world.getBlockSize();
		}

		ska::Point<int> blockColPosY;
		if (!m_world.canMoveToPos(nextPosY, blockColPosY)){
			collided = true;
			wcol.yaxis = true;
			wcol.lastBlockColPosY = lastBlockColPosY;
			wcol.blockColPosY = blockColPosY * m_world.getBlockSize();
		}

		if (collided) {
			WorldCollisionObservable::notifyObservers(CollisionEvent(entityId), wcol, m_entityManager.getComponent<CollidableComponent>(entityId));
		}

		if (entityCollided) {
			entityCollided = false;
			/* When collision between entities is detected, we can do things as decreasing health,
			pushing entities, or any statistic interaction */
			EntityCollisionObservable::notifyObservers(CollisionEvent(entityId), col, m_entityManager.getComponent<CollidableComponent>(entityId));
		}
	}
}

const ska::Rectangle ska::CollisionSystem::createHitBox(ska::EntityId entityId, bool xaxis) {
	PositionComponent& positionComponent = m_entityManager.getComponent<PositionComponent>(entityId);
	HitboxComponent& hitboxComponent = m_entityManager.getComponent<HitboxComponent>(entityId);
	MovementComponent& movementComponent = m_entityManager.getComponent<MovementComponent>(entityId);

	ska::Rectangle hitBox;
	hitBox.x = positionComponent.x + (xaxis ? (movementComponent.vx + movementComponent.ax) : 0) + hitboxComponent.xOffset + 0.5;
	hitBox.y = positionComponent.y + (!xaxis ? (movementComponent.vy + movementComponent.ay) : 0) + hitboxComponent.yOffset + 0.5;
	hitBox.w = hitboxComponent.width;
	hitBox.h = hitboxComponent.height;
	return hitBox;
}

ska::CollisionSystem::~CollisionSystem() {
}
