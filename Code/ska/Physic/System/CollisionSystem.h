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


	class CollisionSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, MovementComponent, HitboxComponent, CollidableComponent> {
	public:
		CollisionSystem(ska::World& w, ska::EntityManager& entityManager, ska::GameEventDispatcher& ged);
		virtual ~CollisionSystem();
	protected:
		virtual void refresh() override;
	private:
		inline const ska::Rectangle createHitBox(ska::EntityId entityId, bool xaxis);
		World& m_world;
		ska::GameEventDispatcher& m_ged;
	};
}
