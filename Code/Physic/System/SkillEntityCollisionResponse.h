#pragma once
#include "../../ska/Physic/System/CollisionSystem.h"
#include "../../ska/Physic/System/EntityCollisionResponse.h"

class SkillEntityCollisionResponse : public ska::EntityCollisionResponse {
public:
	SkillEntityCollisionResponse(ska::CollisionSystem& w, ska::EntityManager& entityManager);
	~SkillEntityCollisionResponse();
	void onEntityCollision(const ska::CollisionEvent& e, ska::CollisionComponent& col, const ska::CollidableComponent& cc);

};

