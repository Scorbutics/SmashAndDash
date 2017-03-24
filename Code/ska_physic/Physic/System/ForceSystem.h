#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../MovementComponent.h"
#include "../ForceComponent.h"

namespace ska {
	class ForceSystem : public System<std::unordered_set<EntityId>, ForceComponent, MovementComponent> {
	public:
		explicit ForceSystem(EntityManager& entityManager);
		void operator=(const ForceSystem& fs) = delete;
		virtual ~ForceSystem();
	protected:
		virtual void refresh() override;

	};
}
