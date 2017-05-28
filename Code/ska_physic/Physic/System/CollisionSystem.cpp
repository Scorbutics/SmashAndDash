#include "CollisionSystem.h"
#include "ECS/Basics/Physic/CollisionProfile.h"
#include "Utils/RectangleUtils.h"
#include "ECS/Basics/Physic/CollisionComponent.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"

ska::CollisionSystem::CollisionSystem(EntityManager& entityManager, CollisionProfile& cp, GameEventDispatcher& ged) :
	System(entityManager),
	m_collisionProfile(cp),
	m_ged(ged) {
	name("CollisionSystem");
}

void ska::CollisionSystem::refresh(unsigned int ellapsedTime) {
	for (auto entityId : m_processed) {

		std::vector<Point<int>> lastBlockColPosX;
		std::vector<Point<int>> lastBlockColPosY;
		if (m_entityManager.hasComponent<WorldCollisionComponent>(entityId)) {
			const auto& wcc = m_entityManager.getComponent<WorldCollisionComponent>(entityId);
			lastBlockColPosX = wcc.blockColPosX;
			lastBlockColPosY = wcc.blockColPosY;
			m_entityManager.removeComponent<WorldCollisionComponent>(entityId);
			/* Il faut rafraîchir l'ensemble des systèmes directement ici (au travers de l'EntityManager), 
			car il est possible de créer un autre WorldCollisionComponent dans le même tour de boucle. 
			Si le refresh n'est pas fait, il sera supprimé par le prochain rafraîchissement de l'EntityManager */
			m_entityManager.refreshEntity(entityId);
		}

		const auto dt = 1.0F;

		const auto entityHitboxX = createHitBox(entityId, dt, true);
		const auto entityHitboxY = createHitBox(entityId, dt, false);
		
		auto entityCollided = false;
		CollisionComponent col;
		for (auto itEntity : m_processed) {
			if (itEntity != entityId) {
				if (RectangleUtils::collisionBoxABoxB(entityHitboxX, createHitBox(itEntity, dt, true))) {
					col.origin = entityId;
					col.target = itEntity;
					entityCollided = true;
					col.xaxis = true;
				}
				if (RectangleUtils::collisionBoxABoxB(entityHitboxY, createHitBox(itEntity, dt, false))) {
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

ska::Rectangle ska::CollisionSystem::createHitBox(EntityId entityId, float dt, bool xaxis) const{
	auto& positionComponent = m_entityManager.getComponent<PositionComponent>(entityId);
	auto& hitboxComponent = m_entityManager.getComponent<HitboxComponent>(entityId);
	auto& movementComponent = m_entityManager.getComponent<MovementComponent>(entityId);

	Rectangle hitBox;
	hitBox.x = static_cast<int>(positionComponent.x + (xaxis ? (movementComponent.vx * dt) : 0) + hitboxComponent.xOffset + 0.5);
	hitBox.y = static_cast<int>(positionComponent.y + (!xaxis ? (movementComponent.vy * dt) : 0) + hitboxComponent.yOffset + 0.5);
	hitBox.w = hitboxComponent.width;
	hitBox.h = hitboxComponent.height;
	return hitBox;
}

ska::CollisionSystem::~CollisionSystem() {
}
