#pragma once
#include "../../ska/Physic/System/CollisionSystem.h"

class SkillCollisionSystem : public  ska::CollisionSystem {
public:
	SkillCollisionSystem(ska::World& w, ska::EntityManager& entityManager);
	virtual void handleEntityCollision(ska::CollisionComponent& col);
	virtual ~SkillCollisionSystem();

};