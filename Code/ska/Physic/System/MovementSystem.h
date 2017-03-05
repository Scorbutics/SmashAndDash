#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../MovementComponent.h"
#include "../PositionComponent.h"
#include "../ForceComponent.h"

namespace ska {
	class MovementSystem : public System<std::unordered_set<EntityId>, PositionComponent, MovementComponent, ForceComponent> {
	public:
		explicit MovementSystem(EntityManager& entityManager);
		MovementSystem& operator=(const MovementSystem&) = delete;
		virtual ~MovementSystem();
	protected:
		virtual void refresh() override;

	};
}
