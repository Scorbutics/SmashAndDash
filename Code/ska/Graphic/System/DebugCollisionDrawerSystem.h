#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../../Physic/WorldCollisionComponent.h"
#include "../DebugGraphicComponent.h"
#include "../../Physic/PositionComponent.h"
#include "../../Physic/HitboxComponent.h"

namespace ska {
	class DebugCollisionDrawerSystem : public System<std::unordered_set<EntityId>, PositionComponent, HitboxComponent, DebugGraphicComponent> {
	public:
		DebugCollisionDrawerSystem(EntityManager& em);
		EntityId createDebugCollisionEntity(const Point<int>& p, int offset) const;
		virtual void refresh() override;
		virtual ~DebugCollisionDrawerSystem() = default;
	};
}
