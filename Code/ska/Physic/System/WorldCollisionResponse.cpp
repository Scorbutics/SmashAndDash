#include <iostream>
#include "../CollisionEvent.h"
#include "WorldCollisionResponse.h"
#include "../WorldCollisionComponent.h"
#include "../CollidableComponent.h"
#include "../CollisionComponent.h"
#include "../../ECS/EntityManager.h"
#include "../../World/World.h"

ska::WorldCollisionResponse::WorldCollisionResponse(World& w, GameEventDispatcher& ged, EntityManager& em) :
	WorldCollisionObserver(bind(&WorldCollisionResponse::onWorldCollision, this, std::placeholders::_1)),
	m_entityManager(em),
	m_ged(ged),
	m_world(w) {
	m_ged.ska::Observable<CollisionEvent>::addObserver(*this);
}

ska::WorldCollisionResponse::WorldCollisionResponse(std::function<bool(CollisionEvent&)> onEntityCollision, World& w, GameEventDispatcher& ged, EntityManager& em) :
WorldCollisionObserver(onEntityCollision),
m_entityManager(em),
m_ged(ged),
m_world(w) {
	m_ged.ska::Observable<CollisionEvent>::addObserver(*this);
}

bool ska::WorldCollisionResponse::onWorldCollision(CollisionEvent& colE) {
	if (colE.wcollisionComponent == nullptr) {
		return false;
	}

	auto wcol = *colE.wcollisionComponent;
	bool colX = false;
	if (wcol.xaxis) {
		for (const auto& p : wcol.blockColPosX) {
			colX |= !m_world.canMoveOnBlock(p, colE.collidableComponent.authorizedBlockIds, 0);
			//colX |= !m_world.canMoveOnBlock(p, cc.authorizedBlockIds, 1);
			if (colX) {
				break;
			}
		}
	}
	
	bool colY = false;
	if (wcol.yaxis) {
		for (const auto& p : wcol.blockColPosY) {
			colY |= !m_world.canMoveOnBlock(p, colE.collidableComponent.authorizedBlockIds, 0);
			//colY |= !m_world.canMoveOnBlock(p, cc.authorizedBlockIds, 1);
			if (colY) {
				break;
			}
		}
	}

	if (colX || colY) {
		m_entityManager.addComponent<WorldCollisionComponent>(colE.entity, wcol);
	}
	return true;
}

ska::WorldCollisionResponse::~WorldCollisionResponse() {
	m_ged.ska::Observable<CollisionEvent>::removeObserver(*this);
}