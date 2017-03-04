#include "CollisionSystem.h"
#include "../../Utils/RectangleUtils.h"
#include "../CollisionComponent.h"
#include "../WorldCollisionComponent.h"

ska::CollisionSystem::CollisionSystem(World& w, EntityManager& entityManager, GameEventDispatcher& ged) : 
	System(entityManager), 
	m_world(w),
	m_ged(ged) {
}

void ska::CollisionSystem::refresh() {
	for (EntityId entityId : m_processed) {
		PositionComponent& positionComponent = m_entityManager.getComponent<PositionComponent>(entityId);
		HitboxComponent& hitboxComponent = m_entityManager.getComponent<HitboxComponent>(entityId);
		MovementComponent& moveComponent = m_entityManager.getComponent<MovementComponent>(entityId);
		
		std::vector<Point<int>> lastBlockColPosX;
		std::vector<Point<int>> lastBlockColPosY;
		if (m_entityManager.hasComponent<WorldCollisionComponent>(entityId)) {
			const WorldCollisionComponent& wcc = m_entityManager.getComponent<WorldCollisionComponent>(entityId);
			lastBlockColPosX = wcc.blockColPosX;
			lastBlockColPosY = wcc.blockColPosY;
			m_entityManager.removeComponent<WorldCollisionComponent>(entityId);
		}

		const Rectangle entityHitboxX = createHitBox(entityId, true);
		const Rectangle entityHitboxY = createHitBox(entityId, false);
		
		bool entityCollided = false;
		CollisionComponent col;
		for (EntityId itEntity : m_processed) {
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
		
		wcol.blockColPosX.clear();
		if (!m_world.canMoveToPos(nextPosX, wcol.blockColPosX)){
			collided = true;
			wcol.xaxis = true;
			wcol.lastBlockColPosX = lastBlockColPosX;
		}

		wcol.blockColPosY.clear();
		if (!m_world.canMoveToPos(nextPosY, wcol.blockColPosY)){
			collided = true;
			wcol.yaxis = true;
			wcol.lastBlockColPosY = lastBlockColPosY;
		}

		if (collided) {
			m_ged.Observable<CollisionEvent>::notifyObservers(CollisionEvent(entityId, &wcol, nullptr, m_entityManager.getComponent<CollidableComponent>(entityId)));
		}

		if (entityCollided) {
			entityCollided = false;
			/* When collision between entities is detected, we can do things as decreasing health,
			pushing entities, or any statistic interaction */
			m_ged.Observable<CollisionEvent>::notifyObservers(CollisionEvent(entityId, nullptr, &col, m_entityManager.getComponent<CollidableComponent>(entityId)));
		}
	}
}

const ska::Rectangle ska::CollisionSystem::createHitBox(EntityId entityId, bool xaxis) const{
	PositionComponent& positionComponent = m_entityManager.getComponent<PositionComponent>(entityId);
	HitboxComponent& hitboxComponent = m_entityManager.getComponent<HitboxComponent>(entityId);
	MovementComponent& movementComponent = m_entityManager.getComponent<MovementComponent>(entityId);

	Rectangle hitBox;
	hitBox.x = static_cast<int>(positionComponent.x + (xaxis ? (movementComponent.vx + movementComponent.ax) : 0) + hitboxComponent.xOffset + 0.5);
	hitBox.y = static_cast<int>(positionComponent.y + (!xaxis ? (movementComponent.vy + movementComponent.ay) : 0) + hitboxComponent.yOffset + 0.5);
	hitBox.w = hitboxComponent.width;
	hitBox.h = hitboxComponent.height;
	return hitBox;
}

ska::CollisionSystem::~CollisionSystem() {
}
