#pragma once
#include "../../ska/Core/GameEventDispatcher.h"
#include "../../ska/Physic/System/WorldCollisionResponse.h"

class WorldEntityCollisionResponse : public ska::WorldCollisionResponse {
public:
	WorldEntityCollisionResponse(ska::World& w, ska::GameEventDispatcher& colSys, ska::EntityManager& entityManager);
	WorldEntityCollisionResponse(std::function<bool(ska::CollisionEvent&)> onWorldCollision, ska::World& w, ska::GameEventDispatcher& colSys, ska::EntityManager& entityManager);
	~WorldEntityCollisionResponse();
	bool onWorldCollision(ska::CollisionEvent& e);

};

