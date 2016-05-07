#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../GravityAffectedComponent.h"
#include "../ForceComponent.h"
#include "../MovementComponent.h"

namespace ska {
	class GravitySystem : public ska::System<std::unordered_set<ska::EntityId>, ForceComponent, GravityAffectedComponent, MovementComponent> {
	public:
		GravitySystem(ska::EntityManager& entityManager);
		virtual ~GravitySystem();
	protected:
		virtual void refresh() override;

	};
}
