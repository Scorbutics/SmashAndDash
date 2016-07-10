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

void ska::WorldCollisionResponse::onWorldCollision(const CollisionEvent& e, WorldCollisionComponent& col, const CollidableComponent& cc) {
	//TODO Other layers
	if (col.xaxis && !m_world.canMoveOnBlock(col.blockColPosX, cc.authorizedBlockIds, 0) || col.yaxis && !m_world.canMoveOnBlock(col.blockColPosY, cc.authorizedBlockIds, 0)) {
		m_entityManager.addComponent<WorldCollisionComponent>(e.entity, col);
	}
}

ska::WorldCollisionResponse::~WorldCollisionResponse() {
	m_collisionSystem.WorldCollisionObservable::removeObserver(*this);
}