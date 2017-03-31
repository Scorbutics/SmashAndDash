#pragma once
#include "Utils/Observer.h"
#include "Data/Events/GameEventDispatcher.h"

namespace ska {
	struct CollisionEvent;
	class CollidableComponent;
	class WorldCollisionComponent;
	using WorldCollisionObserver = Observer<CollisionEvent>;
	class EntityManager;

	class DebugWorldCollisionResponse :
		public WorldCollisionObserver {
	public:
		DebugWorldCollisionResponse(GameEventDispatcher& ged, EntityManager& em);
		DebugWorldCollisionResponse& operator=(const DebugWorldCollisionResponse&) = delete;
		bool onWorldCollision(CollisionEvent& e) const;
		~DebugWorldCollisionResponse();
	private:
		EntityManager& m_entityManager;
		GameEventDispatcher& m_ged;
	};
}
