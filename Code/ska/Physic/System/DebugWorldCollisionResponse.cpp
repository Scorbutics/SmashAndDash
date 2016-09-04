#include <iostream>
#include "../CollisionEvent.h"
#include "DebugWorldCollisionResponse.h"
#include "../../Graphic/DebugGraphicComponent.h"
#include "../WorldCollisionComponent.h"
#include "../CollidableComponent.h"
#include "../../ECS/EntityManager.h"
#include "CollisionSystem.h"

ska::DebugWorldCollisionResponse::DebugWorldCollisionResponse(CollisionSystem& colSys, EntityManager& em) :
WorldCollisionObserver(std::bind(&DebugWorldCollisionResponse::onWorldCollision, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
	m_entityManager(em),
	m_collisionSystem(colSys) {
	m_collisionSystem.WorldCollisionObservable::addObserver(*this);
}

bool ska::DebugWorldCollisionResponse::onWorldCollision(const CollisionEvent& e, WorldCollisionComponent& col, const CollidableComponent& cc) {
	DebugGraphicComponent dgc;
	dgc.typeMask = DebugGraphicType::COLLISION;
	m_entityManager.addComponent<DebugGraphicComponent>(e.entity, dgc);
	return true;
}

ska::DebugWorldCollisionResponse::~DebugWorldCollisionResponse() {
	m_collisionSystem.WorldCollisionObservable::removeObserver(*this);
}