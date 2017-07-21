#pragma once
#include <unordered_set>
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "ECS/Basics/Physic/CollidableComponent.h"
#include "Data/Events/CollisionEvent.h"
#include "Rectangle.h"
#include "ECS/System.h"
#include "Data/Events/GameEventDispatcher.h"

namespace ska {
	class CollisionComponent;
	class WorldCollisionComponent;
    class CollisionProfile;

	class CollisionSystem : public System<std::unordered_set<EntityId>, RequiredComponent<PositionComponent, MovementComponent, HitboxComponent, CollidableComponent>, PossibleComponent<WorldCollisionComponent>> {
	public:
		CollisionSystem(EntityManager& entityManager, CollisionProfile& w, GameEventDispatcher& ged);
		CollisionSystem& operator=(const CollisionSystem&) = delete;
		virtual ~CollisionSystem();
	protected:
		virtual void refresh(unsigned int ellapsedTime) override;
	private:
		inline Rectangle createHitBox(EntityId entityId, bool xaxis) const;
		CollisionProfile& m_collisionProfile;
		GameEventDispatcher& m_ged;
	};
}
