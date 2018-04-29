#pragma once
#include "Data/Events/GameEventDispatcher.h"
#include "Utils/SubObserver.h"

namespace ska {
	class EntityManager;
	class CollisionProfile;
}

class WorldEntityCollisionResponse :
	public ska::SubObserver<ska::CollisionEvent> {
public:
	WorldEntityCollisionResponse(ska::CollisionProfile& w, ska::GameEventDispatcher& colSys, ska::EntityManager& entityManager);
	WorldEntityCollisionResponse& operator=(const WorldEntityCollisionResponse&) = delete;
	~WorldEntityCollisionResponse() = default;

private:
	bool onCollision(ska::CollisionEvent& e);
	ska::EntityManager& m_entityManager;

};

