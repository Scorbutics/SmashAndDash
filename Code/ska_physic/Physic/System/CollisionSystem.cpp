#include "CollisionSystem.h"
#include "ECS/Basics/Physic/CollisionProfile.h"
#include "Utils/RectangleUtils.h"
#include "ECS/Basics/Physic/CollisionComponent.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/Basics/Physic/ForceComponent.h"

ska::CollisionSystem::CollisionSystem(EntityManager& entityManager, CollisionProfile& cp, GameEventDispatcher& ged) :
	System(entityManager),
	m_collisionProfile(cp),
	m_ged(ged) {
}

void ska::CollisionSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	for (auto entityId : processed) {

		std::vector<Point<int>> lastBlockColPosX;
		std::vector<Point<int>> lastBlockColPosY;
		const auto& wccPtr = m_componentPossibleAccessor.get<WorldCollisionComponent>(entityId);
		if (wccPtr != nullptr) {
			const auto& wcc = *wccPtr;
			lastBlockColPosX = wcc.blockColPosX;
			lastBlockColPosY = wcc.blockColPosY;
			m_componentAccessor.remove<WorldCollisionComponent>(entityId);
			/* Il faut rafraîchir l'ensemble des systèmes directement ici (au travers de l'EntityManager),
			car il est possible de créer un autre WorldCollisionComponent dans le même tour de boucle.
			Si le refresh n'est pas fait, il sera supprimé par le prochain rafraîchissement de l'EntityManager */
			refreshEntity(entityId);
		}

		const auto entityHitboxX = createHitBox(entityId, true);
		const auto entityHitboxY = createHitBox(entityId, false);

		auto entityCollided = false;
		CollisionComponent col;
		for (auto itEntity : processed) {
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

		//auto& movementComponent = m_componentAccessor.get<MovementComponent>(entityId);

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
			CollisionEvent ce(entityId, &wcol, nullptr, m_componentAccessor.get<CollidableComponent>(entityId));
			m_ged.ska::Observable<CollisionEvent>::notifyObservers(ce);


		}

		if (entityCollided) {
			/* When collision between entities is detected, we can do things as decreasing health,
			pushing entities, or any statistic interaction */
			CollisionEvent ce(entityId, nullptr, &col, m_componentAccessor.get<CollidableComponent>(entityId));
			m_ged.ska::Observable<CollisionEvent>::notifyObservers(ce);
		}
	}
}

ska::Rectangle ska::CollisionSystem::createHitBox(EntityId entityId, bool xaxis) const{
	auto& positionComponent = m_componentAccessor.get<PositionComponent>(entityId);
	auto& hitboxComponent = m_componentAccessor.get<HitboxComponent>(entityId);
	auto& movementComponent = m_componentAccessor.get<MovementComponent>(entityId);

	Rectangle hitBox;
	hitBox.x = ska::NumberUtils::round(positionComponent.x + (xaxis ? (movementComponent.vx) : 0) + hitboxComponent.xOffset);
	hitBox.y = ska::NumberUtils::round(positionComponent.y + (!xaxis ? (movementComponent.vy) : 0) + hitboxComponent.yOffset);
	hitBox.w = hitboxComponent.width;
	hitBox.h = hitboxComponent.height;
	return hitBox;
}

ska::CollisionSystem::~CollisionSystem() {
}
