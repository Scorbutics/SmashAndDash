#pragma once
#include <unordered_set>
#include "ECS/System.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "Ticked.h"

namespace ska {
	class MovementSystem : public System<std::unordered_set<EntityId>, ska::RequiredComponent<PositionComponent, MovementComponent, ForceComponent>, ska::PossibleComponent<>> {
	public:
		MovementSystem(EntityManager& entityManager, Ticked& ticked);
		MovementSystem& operator=(const MovementSystem&) = delete;
		virtual ~MovementSystem();
	protected:
		virtual void refresh(unsigned int ellapsedTime) override;
	
	private:
		const float m_ticks;
	};
}
