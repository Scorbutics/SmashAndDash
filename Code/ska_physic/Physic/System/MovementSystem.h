#pragma once
#include <unordered_set>
#include "ECS/System.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/ForceComponent.h"

namespace ska {
	class MovementSystem : public System<std::unordered_set<EntityId>, PositionComponent, MovementComponent, ForceComponent> {
	public:
		explicit MovementSystem(EntityManager& entityManager);
		MovementSystem& operator=(const MovementSystem&) = delete;
		virtual ~MovementSystem();
	protected:
		virtual void refresh(unsigned int ellapsedTime) override;

	};
}
