#include <iostream>
#include "../CollisionEvent.h"
#include "WorldCollisionResponse.h"
#include "../WorldCollisionComponent.h"
#include "../CollidableComponent.h"
#include "../../ECS/EntityManager.h"
#include "CollisionSystem.h"
#include "../../World/World.h"

ska::WorldCollisionResponse::WorldCollisionResponse(World& w, CollisionSystem& colSys, EntityManager& em) :
	WorldCollisionObserver(std::bind(&WorldCollisionResponse::onWorldCollision, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
	m_entityManager(em),
	m_collisionSystem(colSys),
	m_world(w) {
	m_collisionSystem.WorldCollisionObservable::addObserver(*this);
}

ska::WorldCollisionResponse::WorldCollisionResponse(std::function<bool(const CollisionEvent&, WorldCollisionComponent&, const CollidableComponent&)> onEntityCollision, World& w, CollisionSystem& colSys, ska::EntityManager& em) :
WorldCollisionObserver(onEntityCollision),
m_entityManager(em),
m_collisionSystem(colSys),
m_world(w) {
	m_collisionSystem.WorldCollisionObservable::addObserver(*this);
}

bool ska::WorldCollisionResponse::onWorldCollision(const CollisionEvent& e, WorldCollisionComponent& col, const CollidableComponent& cc) {

	bool colX = false;
	if (col.xaxis) {
		for (const auto& p : col.blockColPosX) {
			colX |= !m_world.canMoveOnBlock(p, cc.authorizedBlockIds, 0);
			//colX |= !m_world.canMoveOnBlock(p, cc.authorizedBlockIds, 1);
			if (colX) {
				break;
			}
		}
	}
	
	bool colY = false;
	if (col.yaxis) {
		for (const auto& p : col.blockColPosY) {
			colY |= !m_world.canMoveOnBlock(p, cc.authorizedBlockIds, 0);
			//colY |= !m_world.canMoveOnBlock(p, cc.authorizedBlockIds, 1);
			if (colY) {
				break;
			}
		}
	}

	if (colX || colY) {
		m_entityManager.addComponent<WorldCollisionComponent>(e.entity, col);
	}
	return true;
}

ska::WorldCollisionResponse::~WorldCollisionResponse() {
	m_collisionSystem.WorldCollisionObservable::removeObserver(*this);
}