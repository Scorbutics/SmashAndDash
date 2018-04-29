#include "WorldEntityCollisionResponse.h"
#include "Data/Events/CollisionEvent.h"
#include "ECS/EntityManager.h"
#include "../../Gameplay/Fight/SkillComponent.h"

WorldEntityCollisionResponse::WorldEntityCollisionResponse(ska::CollisionProfile& w, ska::GameEventDispatcher& ged, ska::EntityManager& em) :
	SubObserver<ska::CollisionEvent>(std::bind(&WorldEntityCollisionResponse::onCollision, this, std::placeholders::_1), ged),
	m_entityManager(em) {
}

//... Collisions should have a collision group and a skill should not interact with the world
bool WorldEntityCollisionResponse::onCollision(ska::CollisionEvent& e) {
	if (!m_entityManager.hasComponent<SkillComponent>(e.entity)) {
		//WorldCollisionResponse::onWorldCollision(e);
	}
	return true;
}
