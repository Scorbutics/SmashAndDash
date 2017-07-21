#pragma once
#include <unordered_set>
#include "ECS/System.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/Basics/Graphic/DebugGraphicComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"

namespace ska {
	class DebugCollisionDrawerSystem : public System<std::unordered_set<EntityId>, RequiredComponent<PositionComponent, HitboxComponent, DebugGraphicComponent>, PossibleComponent<WorldCollisionComponent>> {
	public:
		DebugCollisionDrawerSystem(EntityManager& em);
		EntityId createDebugCollisionEntity(const Point<int>& p, int offset);
		virtual void refresh(unsigned int ellapsedTime) override;
		virtual ~DebugCollisionDrawerSystem() = default;
	};
}
