#pragma once
#include <unordered_set>
#include "../IARandomMovementComponent.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/System.h"
#include "ECS/Basics/Physic/PositionComponent.h"


namespace ska {
	class IARandomMovementSystem : public System<std::unordered_set<EntityId>, RequiredComponent<IARandomMovementComponent, ForceComponent>, PossibleComponent<PositionComponent, DirectionalAnimationComponent>> {
	public:
		explicit IARandomMovementSystem(EntityManager& entityManager);
		IARandomMovementSystem& operator=(const IARandomMovementSystem&) = delete;
		virtual ~IARandomMovementSystem();
	protected:
		virtual void refresh(unsigned int ellapsedTime) override;

	};
}

