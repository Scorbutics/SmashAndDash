#pragma once
#include <unordered_set>
#include "ECS/System.h"
#include "ECS/Basics/Physic/GravityAffectedComponent.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"

namespace ska {
	class GravitySystem : public System<std::unordered_set<EntityId>, RequiredComponent<ForceComponent, GravityAffectedComponent, MovementComponent>, PossibleComponent<>> {
	public:
		explicit GravitySystem(EntityManager& entityManager);
		GravitySystem& operator=(const GravitySystem&) = delete;
		virtual ~GravitySystem();
	protected:
		virtual void refresh(unsigned int ellapsedTime) override;

	};
}
