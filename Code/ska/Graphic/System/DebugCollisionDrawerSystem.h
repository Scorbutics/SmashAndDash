#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../../Physic/WorldCollisionComponent.h"
#include "../DebugGraphicComponent.h"
#include "../../Physic/PositionComponent.h"
#include "../../Physic/HitboxComponent.h"

namespace ska {
	class DebugCollisionDrawerSystem : public ska::System<std::unordered_set<ska::EntityId>, PositionComponent, HitboxComponent, DebugGraphicComponent> {
	public:
		DebugCollisionDrawerSystem(ska::EntityManager& em);
		ska::EntityId createDebugCollisionEntity(const ska::Point<int>& p, int offset) const;
		virtual void refresh() override;
		virtual ~DebugCollisionDrawerSystem() = default;
	};
}