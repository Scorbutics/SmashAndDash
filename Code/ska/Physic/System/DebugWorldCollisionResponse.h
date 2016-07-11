#pragma once
#include "../../Utils/Observer.h"

namespace ska {
	struct CollisionEvent;
	class CollidableComponent;
	class WorldCollisionComponent;
	using WorldCollisionObserver = Observer<const CollisionEvent&, WorldCollisionComponent&, const CollidableComponent&>;
	class EntityManager;
	class CollisionSystem;

	class DebugWorldCollisionResponse :
		public WorldCollisionObserver {
	public:
		DebugWorldCollisionResponse(CollisionSystem& colSys, EntityManager& em);
		void onWorldCollision(const CollisionEvent& e, WorldCollisionComponent& col, const CollidableComponent& cc);
		~DebugWorldCollisionResponse();
	private:
		EntityManager& m_entityManager;
		CollisionSystem& m_collisionSystem;
	};
}