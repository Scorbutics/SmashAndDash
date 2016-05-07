#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../MovementComponent.h"
#include "../ForceComponent.h"

namespace ska {
	class ForceSystem : public ska::System<std::unordered_set<ska::EntityId>, ForceComponent, MovementComponent> {
	public:
		ForceSystem(ska::EntityManager& entityManager);
		virtual ~ForceSystem();
	protected:
		virtual void refresh() override;

	};
}
