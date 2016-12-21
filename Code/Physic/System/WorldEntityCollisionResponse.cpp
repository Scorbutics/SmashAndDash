#include "WorldEntityCollisionResponse.h"
#include "../../ska/Physic/CollisionComponent.h"
#include "../../ska/Physic/CollisionEvent.h"
#include "../../ska/Physic/WorldCollisionComponent.h"
#include "../../ska/ECS/EntityManager.h"
#include "../../Gameplay/Fight/SkillComponent.h"
#include "../../Gameplay/Fight/BattleComponent.h"

WorldEntityCollisionResponse::WorldEntityCollisionResponse(ska::World& w, ska::GameEventDispatcher& ged, ska::EntityManager& em) :
ska::WorldCollisionResponse(std::bind(&WorldEntityCollisionResponse::onWorldCollision, this, std::placeholders::_1), w, ged, em){
	m_ged.ska::Observable<ska::CollisionEvent>::addObserver(*this);
}

WorldEntityCollisionResponse::WorldEntityCollisionResponse(std::function<bool(ska::CollisionEvent&)> onWorldCollision, ska::World& w, ska::GameEventDispatcher& ged, ska::EntityManager& em) :
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