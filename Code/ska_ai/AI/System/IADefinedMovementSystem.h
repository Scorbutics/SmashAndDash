#pragma once
#include <unordered_set>
#include "../../Script/System/ScriptAutoSystem.h"
#include "../IADefinedMovementComponent.h"
#include "../../Physic/MovementComponent.h"
#include "../../Physic/PositionComponent.h"
#include "../../Physic/HitboxComponent.h"
#include "../../ECS/System.h"


namespace ska {
	class IADefinedMovementSystem : public System<std::unordered_set<EntityId>, IADefinedMovementComponent, MovementComponent, PositionComponent, HitboxComponent>
	{
	public:
		explicit IADefinedMovementSystem(EntityManager& entityManager, ScriptAutoSystem* scriptSystem = nullptr);
		virtual ~IADefinedMovementSystem();
	protected:
		virtual void refresh() override;
	private:
		ScriptAutoSystem* m_scriptSystem;
	};
}

