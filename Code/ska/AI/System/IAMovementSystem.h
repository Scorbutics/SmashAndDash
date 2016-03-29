#pragma once
#include <unordered_set>
#include "../IAMovementComponent.h"
#include "../../Physic/ForceComponent.h"
#include "../../ECS/System.h"


namespace ska {
	class IAMovementSystem : public ska::System<std::unordered_set<ska::EntityId>, IAMovementComponent, ForceComponent>
	{
	public:
		IAMovementSystem(ska::EntityManager& entityManager);
		virtual ~IAMovementSystem();
		virtual void refresh() override;

	};
}

