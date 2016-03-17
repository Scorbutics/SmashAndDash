#pragma once
#include <unordered_set>
#include "../MovementComponent.h"
#include "../PositionComponent.h"
#include "../HitboxComponent.h"
#include "../../Graphic/Rectangle.h"
#include "../../ECS/System.h"
#include "../../World/World.h"

namespace ska {
	class CollisionSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, MovementComponent, HitboxComponent>
	{
	public:
		CollisionSystem(ska::World& w, ska::EntityManager& entityManager);
		virtual ~CollisionSystem();
		virtual void refresh() override;
	private:
		const ska::Rectangle createHitBox(ska::EntityId entityId);
		World& m_world;
	};
}
