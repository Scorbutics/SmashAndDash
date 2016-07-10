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

namespace ska {
	class CollisionComponent;
	class WorldCollisionComponent;

	using WorldCollisionObservable = Observable<const CollisionEvent&, WorldCollisionComponent&, const CollidableComponent&>;
	using EntityCollisionObservable = Observable<const CollisionEvent&, CollisionComponent&, const CollidableComponent&>;

	class CollisionSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, MovementComponent, HitboxComponent, CollidableComponent>,
		public WorldCollisionObservable,
		public EntityCollisionObservable {
	public:
		CollisionSystem(ska::World& w, ska::EntityManager& entityManager);
		virtual ~CollisionSystem();
	protected:
		virtual void refresh() override;
	private:
		inline const ska::Rectangle createHitBox(ska::EntityId entityId, bool xaxis);
		World& m_world;
	};
}
