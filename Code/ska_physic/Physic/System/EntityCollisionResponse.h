#pragma once
#include "Utils/Observer.h"
#include "Data/Events/GameEventDispatcher.h"

namespace ska {
	struct CollisionEvent;
	class CollisionComponent;
	class CollidableComponent;
	class EntityManager;
	using EntityCollisionObserver = Observer<CollisionEvent>;

	class EntityCollisionResponse : public EntityCollisionObserver {
	public:
		EntityCollisionResponse(GameEventDispatcher& colSys, EntityManager& em);
		EntityCollisionResponse(std::function<bool(CollisionEvent&)> onEntityCollision, GameEventDispatcher& colSys, EntityManager& em);
		EntityCollisionResponse& operator=(const EntityCollisionResponse&) = delete;
		virtual bool onEntityCollision(CollisionEvent&);
		~EntityCollisionResponse();

	protected:
		EntityManager& m_entityManager;

	private:
		GameEventDispatcher& m_ged;

	};
}
