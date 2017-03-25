#include "CollisionSystem.h"
#include "ECS/Basics/Physic/CollisionProfile.h"
#include "Utils/RectangleUtils.h"
#include "ECS/Basics/Physic/CollisionComponent.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"

ska::CollisionSystem::CollisionSystem(CollisionProfile& cp, EntityManager& entityManager, GameEventDispatcher& ged) :
	System(entityManager),
	m_collisionProfile(cp),
	m_ged(ged) {
}

void ska::CollisionSystem::refresh() {
	for (auto entityId : m_processed) {

		std::vector<Point<int>> lastBlockColPosX;
		std::vector<Point<int>> lastBlockColPosY;
		if (m_entityManager.hasComponent<WorldCollisionComponent>(entityId)) {
			const auto& wcc = m_entityManager.getComponent<WorldCollisionComponent>(entityId);
			lastBlockColPosX = wcc.blockColPosX;
			lastBlockColPosY = wcc.blockColPosY;
			m_entityManager.removeComponent<WorldCollisionComponent>(entityId);
		}

		const auto entityHitboxX = createHitBox(entityId, true);
		const auto entityHitboxY = createHitBox(entityId, false);

		auto entityCollided = false;
		CollisionComponent col;
		for (auto itEntity : m_processed) {
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
		auto collided = false;
		Rectangle nextPosX = { entityHitboxX.x, entityHitboxX.y, entityHitboxX.w, entityHitboxX.h };
		Rectangle nextPosY = { entityHitboxY.x, entityHitboxY.y, entityHitboxY.w, entityHitboxY.h };

		wcol.blockColPosX.clear();
		if (!m_collisionProfile.canMoveToPos(nextPosX, wcol.blockColPosX)){
			collided = true;
			wcol.xaxis = true;
			wcol.lastBlockColPosX = lastBlockColPosX;
		}

		wcol.blockColPosY.clear();
		if (!m_collisionProfile.canMoveToPos(nextPosY, wcol.blockColPosY)){
			collided = true;
			wcol.yaxis = true;
			wcol.lastBlockColPosY = lastBlockColPosY;
		}

		if (collided) {
			CollisionEvent ce(entityId, &wcol, nullptr, m_entityManager.getComponent<CollidableComponent>(entityId));
			m_ged.ska::Observable<CollisionEvent>::notifyObservers(ce);
		}

		if (entityCollided) {
			/* When collision between entities is detected, we can do things as decreasing health,
			pushing entities, or any statistic interaction */
			CollisionEvent ce(entityId, nullptr, &col, m_entityManager.getComponent<CollidableComponent>(entityId));
			m_ged.ska::Observable<CollisionEvent>::notifyObservers(ce);
		}
	}
}

ska::Rectangle ska::CollisionSystem::createHitBox(EntityId entityId, bool xaxis) const{
	auto& positionComponent = m_entityManager.getComponent<PositionComponent>(entityId);
	auto& hitboxComponent = m_entityManager.getComponent<HitboxComponent>(entityId);
	auto& movementComponent = m_entityManager.getComponent<MovementComponent>(entityId);

	Rectangle hitBox;
	hitBox.x = static_cast<int>(positionComponent.x + (xaxis ? (movementComponent.vx + movementComponent.ax) : 0) + hitboxComponent.xOffset + 0.5);
	hitBox.y = static_cast<int>(positionComponent.y + (!xaxis ? (movementComponent.vy + movementComponent.ay) : 0) + hitboxComponent.yOffset + 0.5);
	hitBox.w = hitboxComponent.width;
	hitBox.h = hitboxComponent.height;
	return hitBox;
}

ska::CollisionSystem::~CollisionSystem() {
}
