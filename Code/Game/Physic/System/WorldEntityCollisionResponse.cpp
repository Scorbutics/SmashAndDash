#include "WorldEntityCollisionResponse.h"
#include "Data/Events/CollisionEvent.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/EntityManager.h"
#include "../../Gameplay/Fight/SkillComponent.h"

WorldEntityCollisionResponse::WorldEntityCollisionResponse(ska::CollisionProfile& w, ska::GameEventDispatcher& ged, ska::EntityManager& em) :
WorldCollisionResponse(bind(&WorldEntityCollisionResponse::onWorldCollision, this, std::placeholders::_1), w, ged, em){
	m_ged.ska::Observable<ska::CollisionEvent>::addObserver(*this);
}

WorldEntityCollisionResponse::WorldEntityCollisionResponse(std::function<bool(ska::CollisionEvent&)> onWorldCollision, ska::CollisionProfile& w, ska::GameEventDispatcher& ged, ska::EntityManager& em) :
WorldCollisionResponse(onWorldCollision, w, ged, em) {

}

bool WorldEntityCollisionResponse::onWorldCollision(ska::CollisionEvent& e) {
	if (!m_entityManager.hasComponent<SkillComponent>(e.entity)) {
		WorldCollisionResponse::onWorldCollision(e);
	}
	return true;
}

WorldEntityCollisionResponse::~WorldEntityCollisionResponse() {
	m_ged.ska::Observable<ska::CollisionEvent>::removeObserver(*this);
}
