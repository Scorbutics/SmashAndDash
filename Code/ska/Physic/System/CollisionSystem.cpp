#include "CollisionSystem.h"
#include "../../Utils/RectangleUtils.h"
#include "../CollisionComponent.h"

ska::CollisionSystem::CollisionSystem(ska::World& w, ska::EntityManager& entityManager) : System(entityManager), m_world(w) {
}

void ska::CollisionSystem::refresh() {
	for (ska::EntityId entityId : m_processed) {
		PositionComponent& positionComponent = m_entityManager.getComponent<PositionComponent>(entityId);
		HitboxComponent& hitboxComponent = m_entityManager.getComponent<HitboxComponent>(entityId);

		const ska::Rectangle entityHitboxX = createHitBox(entityId, true);
		const ska::Rectangle entityHitboxY = createHitBox(entityId, false);

		CollisionComponent col;
		bool collided = false;

		std::unordered_set<ska::EntityId>& entities = m_processed;
		for (ska::EntityId itEntity : entities) {
			col.origin = entityId;
			col.target = itEntity;

			if (itEntity != entityId) {
				if (RectangleUtils::collisionBoxABoxB(entityHitboxX, createHitBox(itEntity, true))) {
					collided = true;
					col.entities = true;
					col.xaxis = true;
				}
				
				if (RectangleUtils::collisionBoxABoxB(entityHitboxY, createHitBox(itEntity, false))) {
					collided = true;
					col.entities = true;
					col.yaxis = true;
				}

			}
			
		}

		Rectangle nextPosX = { entityHitboxX.x, entityHitboxX.y, entityHitboxX.w, entityHitboxX.h };
		Rectangle nextPosY = { entityHitboxY.x, entityHitboxY.y, entityHitboxY.w, entityHitboxY.h };
		if (!m_world.canMoveToPos(nextPosX)){
			collided = true;
			col.world = true;
			col.xaxis = true;
		}

		if (!m_world.canMoveToPos(nextPosY)){
			collided = true;
			col.world = true;
			col.yaxis = true;
		}

		if (collided) {
			m_entityManager.addComponent<CollisionComponent>(entityId, col);
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
