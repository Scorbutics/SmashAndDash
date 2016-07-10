#pragma once
#include "../../Utils/Observer.h"

namespace ska {
	struct CollisionEvent;
	class CollisionSystem;
	class CollisionComponent;
	class CollidableComponent;
	class EntityManager;
	using EntityCollisionObserver = Observer<const CollisionEvent&, CollisionComponent&, const CollidableComponent&>;

	class EntityCollisionResponse : public EntityCollisionObserver {
	public:
		EntityCollisionResponse(CollisionSystem& colSys, EntityManager& em);
		void onEntityCollision(const CollisionEvent& e, CollisionComponent& col, const CollidableComponent& cc);
		~EntityCollisionResponse();
	private:
		EntityManager& m_entityManager;
		CollisionSystem& m_collisionSystem;

	};
}