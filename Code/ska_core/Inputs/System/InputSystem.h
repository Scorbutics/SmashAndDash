#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../../ECS/Basics/Input/InputComponent.h"
#include "../../ECS/Basics/Physic/ForceComponent.h"
#include "../../ECS/Basics/Physic/PositionComponent.h"
#include "../InputContextManager.h"

namespace ska {
	class InputSystem : public System<std::unordered_set<EntityId>, RequiredComponent<PositionComponent, InputComponent, ForceComponent>, PossibleComponent<>> {
	public:
		InputSystem(EntityManager& entityManager, const InputContextManager& icm);
		InputSystem& operator=(const InputSystem&) = delete;
		virtual ~InputSystem();
	protected:
		virtual void refresh(unsigned int ellapsedTime) override;
	private:
		const InputContextManager& m_icm;
	};
}
