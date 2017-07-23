#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../../ECS/Basics/Input/InputComponent.h"
#include "../../ECS/Basics/Physic/ForceComponent.h"
#include "../../ECS/Basics/Physic/PositionComponent.h"
#include "../InputContextManager.h"

namespace ska {
	struct InputKeyEvent;

	class InputSystem : 
		public System<std::unordered_set<EntityId>, RequiredComponent<PositionComponent, InputComponent, ForceComponent>, PossibleComponent<>>,
		public Observer<InputKeyEvent> {
	public:
		InputSystem(EntityManager& entityManager, GameEventDispatcher& ged);
		InputSystem& operator=(const InputSystem&) = delete;
		virtual ~InputSystem();
	
	private:
		bool onKeyInputEvent(InputKeyEvent & ike);
		GameEventDispatcher& m_eventDispatcher;

	protected:
		virtual void refresh(unsigned int ellapsedTime) override;
	};
}
