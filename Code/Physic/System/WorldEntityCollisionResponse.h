#pragma once
#include "../../ska/Physic/System/CollisionSystem.h"
#include "../../ska/Physic/System/WorldCollisionResponse.h"

class WorldEntityCollisionResponse : public ska::WorldCollisionResponse {
public:
	WorldEntityCollisionResponse(ska::World& w, ska::CollisionSystem& colSys, ska::EntityManager& entityManager);
	WorldEntityCollisionResponse(std::function<void(const ska::CollisionEvent&, ska::WorldCollisionComponent&, const ska::CollidableComponent&)> onWorldCollision, ska::World& w, ska::CollisionSystem& colSys, ska::EntityManager& entityManager);
	~WorldEntityCollisionResponse();
	void onWorldCollision(const ska::CollisionEvent& e, ska::WorldCollisionComponent& col, const ska::CollidableComponent& cc);

};

