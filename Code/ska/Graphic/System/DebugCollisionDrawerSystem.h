#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../../Physic/WorldCollisionComponent.h"
#include "../DebugCollisionGraphicComponent.h"

namespace ska {
	class DebugCollisionDrawerSystem : public ska::System<std::unordered_set<ska::EntityId>, WorldCollisionComponent, DebugCollisionGraphicComponent> {
	public:
		DebugCollisionDrawerSystem(ska::EntityManager& em);
		ska::EntityId createDebugCollisionEntity(const ska::Point<int>& p);
		virtual void refresh() override;
		virtual ~DebugCollisionDrawerSystem() = default;
	};
}