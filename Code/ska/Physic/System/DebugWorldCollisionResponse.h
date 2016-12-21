#pragma once
#include "../../Utils/Observer.h"
#include "../../Core/GameEventDispatcher.h"

namespace ska {
	struct CollisionEvent;
	class CollidableComponent;
	class WorldCollisionComponent;
	using WorldCollisionObserver = Observer<CollisionEvent>;
	class EntityManager;

	class DebugWorldCollisionResponse :
		public WorldCollisionObserver {
	public:
		DebugWorldCollisionResponse(ska::GameEventDispatcher& ged, EntityManager& em);
		bool onWorldCollision(CollisionEvent& e);
		~DebugWorldCollisionResponse();
	private:
		EntityManager& m_entityManager;
		ska::GameEventDispatcher& m_ged;
	};
}