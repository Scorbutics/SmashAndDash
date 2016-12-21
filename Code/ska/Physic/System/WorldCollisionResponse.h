#pragma once

#include "../../Core/GameEventDispatcher.h"
#include "../../Utils/Observer.h"

namespace ska {
	struct CollisionEvent;
	class CollidableComponent;
	class WorldCollisionComponent;
	class World;
	using WorldCollisionObserver = Observer<CollisionEvent>;
	class EntityManager;
	class CollisionSystem;

	class WorldCollisionResponse : 
		public WorldCollisionObserver {
	public:
		WorldCollisionResponse(World& w, GameEventDispatcher& ged, EntityManager& em);
		WorldCollisionResponse(std::function<bool(CollisionEvent&)> onEntityCollision, World& w, GameEventDispatcher& colSys, ska::EntityManager& em);
		bool onWorldCollision(CollisionEvent& e);
		~WorldCollisionResponse();
	protected:
		EntityManager& m_entityManager;
		GameEventDispatcher& m_ged;
		World& m_world;
	};
}