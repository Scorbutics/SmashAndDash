#pragma once


#include "../../Utils/Observer.h"

namespace ska {
	struct CollisionEvent;
	class CollidableComponent;
	class WorldCollisionComponent;
	class World;
	using WorldCollisionObserver = Observer<const CollisionEvent&, WorldCollisionComponent&, const CollidableComponent&>;
	class EntityManager;
	class CollisionSystem;

	class WorldCollisionResponse : 
		public WorldCollisionObserver {
	public:
		WorldCollisionResponse(World& w, CollisionSystem& colSys, EntityManager& em);
		WorldCollisionResponse(std::function<void(const CollisionEvent&, WorldCollisionComponent&, const CollidableComponent&)> onEntityCollision, World& w, CollisionSystem& colSys, ska::EntityManager& em);
		void onWorldCollision(const CollisionEvent& e, WorldCollisionComponent& col, const CollidableComponent& cc);
		~WorldCollisionResponse();
	protected:
		EntityManager& m_entityManager;
		CollisionSystem& m_collisionSystem;
		World& m_world;
	};
}