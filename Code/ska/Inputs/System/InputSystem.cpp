#include "InputSystem.h"

ska::InputSystem::InputSystem(const InputContextManager& icm, ska::EntityManager& entityManager) : System(entityManager), m_icm(icm) {
}

void ska::InputSystem::refresh() {
	for (EntityId entityId : m_processed) {
		InputComponent& inputComponent = m_entityManager.getComponent<InputComponent>(entityId);
		ForceComponent& forceComponent = m_entityManager.getComponent<ForceComponent>(entityId);
		
		const InputToggleContainer& itc = m_icm.getToggles();
		if (itc[InputToggle::MoveUp]) {
			forceComponent.y -= inputComponent.movePower;
		}

		if (itc[InputToggle::MoveLeft]) {
			forceComponent.x -= inputComponent.movePower;
		}

		if (itc[InputToggle::MoveDown]) {
			forceComponent.y += inputComponent.movePower;
		}

		if (itc[InputToggle::MoveRight]) {
			forceComponent.x += inputComponent.movePower;
		}
	}
}

ska::InputSystem::~InputSystem()
{
}
