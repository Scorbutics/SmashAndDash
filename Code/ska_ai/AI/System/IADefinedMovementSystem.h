#pragma once
#include <unordered_set>
#include "../IADefinedMovementComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "ECS/System.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"


namespace ska {
    class ScriptRegisterer;
	class IADefinedMovementSystem : public System<std::unordered_set<EntityId>, RequiredComponent<IADefinedMovementComponent, MovementComponent, PositionComponent, HitboxComponent>, PossibleComponent<WorldCollisionComponent>> {
	public:
		explicit IADefinedMovementSystem(EntityManager& entityManager, ScriptRegisterer* scriptSystem = nullptr);
		virtual ~IADefinedMovementSystem();
	protected:
		virtual void refresh(unsigned int ellapsedTime) override;
	private:
		ScriptRegisterer* m_scriptSystem;
	};
}

