#pragma once
#include <unordered_set>
#include "../../Script/System/ScriptAutoSystem.h"
#include "../IADefinedMovementComponent.h"
#include "../../Physic/MovementComponent.h"
#include "../../Physic/PositionComponent.h"
#include "../../Physic/HitboxComponent.h"
#include "../../ECS/System.h"


namespace ska {
	class IADefinedMovementSystem : public ska::System<std::unordered_set<ska::EntityId>, IADefinedMovementComponent, MovementComponent, PositionComponent, HitboxComponent>
	{
	public:
		IADefinedMovementSystem(ska::EntityManager& entityManager, ska::ScriptAutoSystem* scriptSystem = nullptr);
		virtual ~IADefinedMovementSystem();
	protected:
		virtual void refresh() override;
	private:
		ska::ScriptAutoSystem* m_scriptSystem;
	};
}

