#pragma once
#include <unordered_set>
#include "../IARandomMovementComponent.h"
#include "../../Physic/ForceComponent.h"
#include "../../ECS/System.h"


namespace ska {
	class IARandomMovementSystem : public ska::System<std::unordered_set<ska::EntityId>, IARandomMovementComponent, ForceComponent>
	{
	public:
		IARandomMovementSystem(ska::EntityManager& entityManager);
		virtual ~IARandomMovementSystem();
	protected:
		virtual void refresh() override;

	};
}

