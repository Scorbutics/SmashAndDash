#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../GravityAffectedComponent.h"
#include "../ForceComponent.h"
#include "../MovementComponent.h"

namespace ska {
	class GravitySystem : public System<std::unordered_set<EntityId>, ForceComponent, GravityAffectedComponent, MovementComponent> {
	public:
		explicit GravitySystem(EntityManager& entityManager);
		GravitySystem& operator=(const GravitySystem&) = delete;
		virtual ~GravitySystem();
	protected:
		virtual void refresh() override;

	};
}
