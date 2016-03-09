#include "CollisionSystem.h"
#include "../../ska/Graphic/Rectangle.h"
#include "../../ska/Utils/RectangleUtils.h"

CollisionSystem::CollisionSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void CollisionSystem::refresh(ska::EntityId& entityId) {
	PositionComponent& positionComponent = m_entityManager.getComponent<PositionComponent>(entityId);
	MovementComponent& movementComponent = m_entityManager.getComponent<MovementComponent>(entityId);
	HitboxComponent& hitboxComponent = m_entityManager.getComponent<HitboxComponent>(entityId);

	const ska::Rectangle entityHitbox = createHitBox(entityId);
	std::unordered_set<ska::EntityId>& entities = getProcessedEntities();
	for (ska::EntityId itEntity : entities) {
		if (ska::RectangleUtils::collisionBoxABoxB(entityHitbox, createHitBox(itEntity))) {

		} else if(false/* TODO World collision => rollback la position */){
			positionComponent.x -= movementComponent.vx;
			positionComponent.y -= movementComponent.vy;
			positionComponent.z -= movementComponent.vz;
		}
	}
}

const ska::Rectangle CollisionSystem::createHitBox(ska::EntityId entityId) {
	PositionComponent& positionComponent = m_entityManager.getComponent<PositionComponent>(entityId);
	HitboxComponent& hitboxComponent = m_entityManager.getComponent<HitboxComponent>(entityId);

	ska::Rectangle hitBox;
	hitBox.x = positionComponent.x + hitboxComponent.xOffset;
	hitBox.y = positionComponent.y + hitboxComponent.yOffset;
	hitBox.w = hitboxComponent.width;
	hitBox.h = hitboxComponent.height;
	return hitBox;
}

CollisionSystem::~CollisionSystem() {
}
