#pragma once
#include <unordered_set>
#include "../IARandomMovementComponent.h"
#include "../../Physic/ForceComponent.h"
#include "../../ECS/System.h"


namespace ska {
	class IARandomMovementSystem : public System<std::unordered_set<EntityId>, IARandomMovementComponent, ForceComponent> {
	public:
		explicit IARandomMovementSystem(EntityManager& entityManager);
		IARandomMovementSystem& operator=(const IARandomMovementSystem&) = delete;
		virtual ~IARandomMovementSystem();
	protected:
		virtual void refresh() override;

	};
}

