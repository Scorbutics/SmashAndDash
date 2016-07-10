#pragma once
#include "../../ska/Physic/System/CollisionSystem.h"
#include "../../ska/Utils/Observer.h"

namespace ska {
	struct CollisionEvent;
	class CollisionSystem;
	class CollisionComponent;
	class CollidableComponent;
	class EntityManager;
	using EntityCollisionObserver = Observer<const CollisionEvent&, CollisionComponent&, const CollidableComponent&>;
}

class SkillEntityCollisionResponse : public ska::EntityCollisionObserver {
public:
	SkillEntityCollisionResponse(ska::CollisionSystem& w, ska::EntityManager& entityManager);
	~SkillEntityCollisionResponse();
	void onEntityCollision(const ska::CollisionEvent& e, ska::CollisionComponent& col, const ska::CollidableComponent& cc);
private:
	ska::EntityManager& m_entityManager;
	ska::CollisionSystem& m_collisionSystem;

};

