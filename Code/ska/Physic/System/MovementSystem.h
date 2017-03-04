#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../MovementComponent.h"
#include "../PositionComponent.h"
#include "../ForceComponent.h"

namespace ska {
	class MovementSystem : public System<std::unordered_set<EntityId>, PositionComponent, MovementComponent, ForceComponent> {
	public:
		MovementSystem(EntityManager& entityManager);
		virtual ~MovementSystem();
	protected:
		virtual void refresh() override;

	};
}
