#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../MovementComponent.h"
#include "../PositionComponent.h"
#include "../ForceComponent.h"

namespace ska {
	class MovementSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, MovementComponent, ForceComponent> {
	public:
		MovementSystem(ska::EntityManager& entityManager);
		virtual ~MovementSystem();
		virtual void refresh() override;

	};
}
