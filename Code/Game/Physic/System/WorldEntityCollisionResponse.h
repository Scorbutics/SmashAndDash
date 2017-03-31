#pragma once
#include "Data/Events/GameEventDispatcher.h"
#include "Physic/System/WorldCollisionResponse.h"

namespace ska {
    class CollisionProfile;
}

class WorldEntityCollisionResponse : public ska::WorldCollisionResponse {
public:
	WorldEntityCollisionResponse(ska::CollisionProfile& w, ska::GameEventDispatcher& colSys, ska::EntityManager& entityManager);
	WorldEntityCollisionResponse& operator=(const WorldEntityCollisionResponse&) = delete;
	WorldEntityCollisionResponse(std::function<bool(ska::CollisionEvent&)> onWorldCollision, ska::CollisionProfile& w, ska::GameEventDispatcher& colSys, ska::EntityManager& entityManager);
	~WorldEntityCollisionResponse();
	bool onWorldCollision(ska::CollisionEvent& e);

};

