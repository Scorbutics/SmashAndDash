#include "WorldEntityCollisionResponse.h"
#include "../../ska/Physic/CollisionComponent.h"
#include "../../ska/Physic/WorldCollisionComponent.h"
#include "../../Gameplay/Fight/SkillComponent.h"
#include "../../Gameplay/Fight/BattleComponent.h"

WorldEntityCollisionResponse::WorldEntityCollisionResponse(ska::World& w, ska::CollisionSystem& colSys, ska::EntityManager& em) :
ska::WorldCollisionResponse(std::bind(&WorldEntityCollisionResponse::onWorldCollision, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), w, colSys, em){
	m_collisionSystem.ska::WorldCollisionObservable::addObserver(*this);
}

WorldEntityCollisionResponse::WorldEntityCollisionResponse(std::function<bool(const ska::CollisionEvent&, ska::WorldCollisionComponent&, const ska::CollidableComponent&)> onWorldCollision, ska::World& w, ska::CollisionSystem& colSys, ska::EntityManager& em) :
WorldCollisionResponse(onWorldCollision, w, colSys, em) {

}

bool WorldEntityCollisionResponse::onWorldCollision(const ska::CollisionEvent& e, ska::WorldCollisionComponent& col, const ska::CollidableComponent& cc) {
	if (!m_entityManager.hasComponent<SkillComponent>(e.entity)) {
		WorldCollisionResponse::onWorldCollision(e, col, cc);
	}
	return true;
}

WorldEntityCollisionResponse::~WorldEntityCollisionResponse() {
	m_collisionSystem.ska::WorldCollisionObservable::removeObserver(*this);
}