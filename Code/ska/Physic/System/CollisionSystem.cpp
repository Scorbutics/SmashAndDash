#include "CollisionSystem.h"
#include "../../Utils/RectangleUtils.h"
#include "../CollisionComponent.h"

ska::CollisionSystem::CollisionSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void ska::CollisionSystem::refresh() {
	for (ska::EntityId entityId : m_processed) {
		ska::PositionComponent& positionComponent = m_entityManager.getComponent<ska::PositionComponent>(entityId);
		MovementComponent& movementComponent = m_entityManager.getComponent<MovementComponent>(entityId);
		HitboxComponent& hitboxComponent = m_entityManager.getComponent<HitboxComponent>(entityId);

		const ska::Rectangle entityHitbox = createHitBox(entityId);
		std::unordered_set<ska::EntityId>& entities = m_processed;
		for (ska::EntityId itEntity : entities) {
			if (ska::RectangleUtils::collisionBoxABoxB(entityHitbox, createHitBox(itEntity))) {
				CollisionComponent col;
				col.origin = entityId;
				col.target = itEntity;
				col.world = false;
				m_entityManager.addComponent<CollisionComponent>(entityId, col);
			}
			else if (false/* TODO World collision check */){
				CollisionComponent col;
				col.origin = entityId;
				col.world = true;
				col.target = 0;
				m_entityManager.addComponent<CollisionComponent>(entityId, col);
			}
		}
	}
}

const ska::Rectangle ska::CollisionSystem::createHitBox(ska::EntityId entityId) {
	ska::PositionComponent& positionComponent = m_entityManager.getComponent<ska::PositionComponent>(entityId);
	HitboxComponent& hitboxComponent = m_entityManager.getComponent<HitboxComponent>(entityId);

	ska::Rectangle hitBox;
	hitBox.x = positionComponent.x + hitboxComponent.xOffset;
	hitBox.y = positionComponent.y + hitboxComponent.yOffset;
	hitBox.w = hitboxComponent.width;
	hitBox.h = hitboxComponent.height;
	return hitBox;
}

ska::CollisionSystem::~CollisionSystem() {
}
