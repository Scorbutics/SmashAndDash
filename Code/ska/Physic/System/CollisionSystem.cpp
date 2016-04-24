#include "CollisionSystem.h"
#include "../../Utils/RectangleUtils.h"
#include "../CollisionComponent.h"
#include "../ForceComponent.h"
#include "../WorldCollisionComponent.h"

ska::CollisionSystem::CollisionSystem(ska::World& w, ska::EntityManager& entityManager) : System(entityManager), m_world(w) {
}

void ska::CollisionSystem::refresh() {
	for (ska::EntityId entityId : m_processed) {
		PositionComponent& positionComponent = m_entityManager.getComponent<PositionComponent>(entityId);
		HitboxComponent& hitboxComponent = m_entityManager.getComponent<HitboxComponent>(entityId);
		MovementComponent& moveComponent = m_entityManager.getComponent<MovementComponent>(entityId);

		const ska::Rectangle entityHitboxX = createHitBox(entityId, true);
		const ska::Rectangle entityHitboxY = createHitBox(entityId, false);

		for (ska::EntityId itEntity : m_processed) {
			CollisionComponent col;
			bool collided = false;
			if (itEntity != entityId) {
				if (RectangleUtils::collisionBoxABoxB(entityHitboxX, createHitBox(itEntity, true))) {
					col.origin = entityId;
					col.target = itEntity;
					collided = true;
					col.xaxis = true;
				} 
				if (RectangleUtils::collisionBoxABoxB(entityHitboxY, createHitBox(itEntity, false))) {
					col.origin = entityId;
					col.target = itEntity;
					collided = true;
					col.yaxis = true;
				}

			}

			if (collided) {
				collided = false;
				
				/* When collision between entities is detected, we can do things as decreasing health, 
				pushing entities, or any statistic interaction */
				handleEntityCollision(col);

				break;
			}

		}

		WorldCollisionComponent col;
		bool collided = false;
		Rectangle nextPosX = { entityHitboxX.x, entityHitboxX.y, entityHitboxX.w, entityHitboxX.h };
		Rectangle nextPosY = { entityHitboxY.x, entityHitboxY.y, entityHitboxY.w, entityHitboxY.h };
		if (!m_world.canMoveToPos(nextPosX)){
			collided = true;
			col.xaxis = true;
		}

		if (!m_world.canMoveToPos(nextPosY)){
			collided = true;
			col.yaxis = true;
		}

		if (collided) {
			m_entityManager.addComponent<WorldCollisionComponent>(entityId, col);
		}
	}
}

void ska::CollisionSystem::handleEntityCollision(ska::CollisionComponent& col) {
	ForceComponent& ftarget = m_entityManager.getComponent<ForceComponent>(col.target);
	MovementComponent& mtarget = m_entityManager.getComponent<MovementComponent>(col.target);

	ForceComponent& forigin = m_entityManager.getComponent<ForceComponent>(col.origin);
	MovementComponent& moveComponent = m_entityManager.getComponent<MovementComponent>(col.origin);

	if (col.xaxis) {
		ftarget.x += (moveComponent.vx + moveComponent.ax)*ftarget.weight;
	}

	if (col.yaxis) {
		ftarget.y += (moveComponent.vy + moveComponent.ay)*ftarget.weight;
	}

	if (col.xaxis) {
		forigin.x = -ftarget.x / 2;
	}

	if (col.yaxis) {
		forigin.y = -ftarget.y / 2;
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
