#pragma once
#include <unordered_set>
#include "../MovementComponent.h"
#include "../PositionComponent.h"
#include "../HitboxComponent.h"
#include "../CollidableComponent.h"
#include "../CollisionEvent.h"
#include "../../Graphic/Rectangle.h"
#include "../../ECS/System.h"
#include "../../World/World.h"
#include "../../Core/GameEventDispatcher.h"

namespace ska {
	class CollisionComponent;
	class WorldCollisionComponent;


	class CollisionSystem : public System<std::unordered_set<EntityId>, PositionComponent, MovementComponent, HitboxComponent, CollidableComponent> {
	public:
		CollisionSystem(World& w, EntityManager& entityManager, GameEventDispatcher& ged);
		CollisionSystem& operator=(const CollisionSystem&) = delete;
		virtual ~CollisionSystem();
	protected:
		virtual void refresh() override;
	private:
		inline Rectangle createHitBox(EntityId entityId, bool xaxis) const;
		World& m_world;
		GameEventDispatcher& m_ged;
	};
}
