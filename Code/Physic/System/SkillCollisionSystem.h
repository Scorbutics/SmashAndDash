#pragma once
#include "../../ska/Physic/System/CollisionSystem.h"

class SkillCollisionSystem : public  ska::CollisionSystem {
public:
	SkillCollisionSystem(ska::World& w, ska::EntityManager& entityManager);
	virtual void handleEntityCollision(ska::CollisionComponent& col) override;
	virtual void handleWorldCollision(ska::WorldCollisionComponent& col, ska::EntityId e) override;
	virtual ~SkillCollisionSystem();

};