#include "Data/Events/CollisionEvent.h"
#include "WorldCollisionResponse.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/Basics/Physic/CollidableComponent.h"
#include "ECS/EntityManager.h"
#include "ECS/Basics/Physic/CollisionProfile.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"

ska::WorldCollisionResponse::WorldCollisionResponse(CollisionProfile& cp, GameEventDispatcher& ged, EntityManager& em) :
	WorldCollisionObserver(bind(&WorldCollisionResponse::onWorldCollision, this, std::placeholders::_1)),
	m_entityManager(em),
	m_ged(ged),
	m_collisionProfile(cp) {
	m_ged.ska::Observable<CollisionEvent>::addObserver(*this);
}

ska::WorldCollisionResponse::WorldCollisionResponse(std::function<bool(CollisionEvent&)> onEntityCollision, CollisionProfile& cp, GameEventDispatcher& ged, EntityManager& em) :
WorldCollisionObserver(onEntityCollision),
m_entityManager(em),
m_ged(ged),
m_collisionProfile(cp) {
	m_ged.ska::Observable<CollisionEvent>::addObserver(*this);
}

bool ska::WorldCollisionResponse::onWorldCollision(CollisionEvent& colE) {
	if (colE.wcollisionComponent == nullptr) {
		return false;
	}

	auto wcol = *colE.wcollisionComponent;
	auto colX = false;
	if (wcol.xaxis) {
		for (const auto& p : wcol.blockColPosX) {
			colX |= !m_collisionProfile.canMoveOnBlock(p, colE.collidableComponent.authorizedBlockIds, 0);
			//colX |= !m_world.canMoveOnBlock(p, cc.authorizedBlockIds, 1);
			if (colX) {
				auto& movementComponent = m_entityManager.getComponent<ska::MovementComponent>(colE.entity);
				auto& forceComponent = m_entityManager.getComponent<ska::ForceComponent>(colE.entity);
				movementComponent.vx = 0;
				movementComponent.ax = 0;
				forceComponent.x = 0;


				break;
			}
		}
	}

	auto colY = false;
	if (wcol.yaxis) {
		for (const auto& p : wcol.blockColPosY) {
			colY |= !m_collisionProfile.canMoveOnBlock(p, colE.collidableComponent.authorizedBlockIds, 0);
			//colY |= !m_world.canMoveOnBlock(p, cc.authorizedBlockIds, 1);
			if (colY) {
				auto& movementComponent = m_entityManager.getComponent<ska::MovementComponent>(colE.entity);
				auto& forceComponent = m_entityManager.getComponent<ska::ForceComponent>(colE.entity);
				movementComponent.vy = 0;
				movementComponent.ay = 0;
				forceComponent.y = 0;
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
