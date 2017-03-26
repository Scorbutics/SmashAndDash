#pragma once
#include <unordered_set>
#include "ECS/System.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/Basics/Graphic/DebugGraphicComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"

namespace ska {
	class DebugCollisionDrawerSystem : public System<std::unordered_set<EntityId>, PositionComponent, HitboxComponent, DebugGraphicComponent> {
	public:
		DebugCollisionDrawerSystem(EntityManager& em);
		EntityId createDebugCollisionEntity(const Point<int>& p, int offset) const;
		virtual void refresh() override;
		virtual ~DebugCollisionDrawerSystem() = default;
	};
}
