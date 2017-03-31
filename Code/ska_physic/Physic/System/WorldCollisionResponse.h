#pragma once

#include "Data/Events/GameEventDispatcher.h"
#include "Utils/Observer.h"

namespace ska {
	struct CollisionEvent;
	class CollidableComponent;
	class WorldCollisionComponent;
	class CollisionProfile;
	using WorldCollisionObserver = Observer<CollisionEvent>;
	class EntityManager;
	class CollisionSystem;

	class WorldCollisionResponse :
		public WorldCollisionObserver {
	public:
		WorldCollisionResponse(CollisionProfile& cp, GameEventDispatcher& ged, EntityManager& em);
		WorldCollisionResponse(std::function<bool(CollisionEvent&)> onEntityCollision, CollisionProfile& cp, GameEventDispatcher& colSys, EntityManager& em);
		WorldCollisionResponse& operator=(const WorldCollisionResponse&) = delete;
		virtual bool onWorldCollision(CollisionEvent& e);
		~WorldCollisionResponse();
	protected:
		EntityManager& m_entityManager;
		GameEventDispatcher& m_ged;
		CollisionProfile& m_collisionProfile;
	};
}
