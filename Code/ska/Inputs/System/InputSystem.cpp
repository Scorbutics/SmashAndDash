#include "InputSystem.h"

ska::InputSystem::InputSystem(const InputContextManager& icm, ska::EntityManager& entityManager) : System(entityManager), m_icm(icm) {
}

void ska::InputSystem::refresh() {
	for (EntityId entityId : m_processed) {
		InputComponent& inputComponent = m_entityManager.getComponent<InputComponent>(entityId);
		ForceComponent& forceComponent = m_entityManager.getComponent<ForceComponent>(entityId);
		
		Point<float> movePower;
		bool moveX = false;
		bool moveY = false;
		const float factor = 0.709;
		const InputToggleContainer& itc = m_icm.getToggles();

		if (itc[InputToggle::MoveUp]) {
			movePower.y = -(int)inputComponent.movePower;
			moveY = true;
		}

		if (itc[InputToggle::MoveLeft]) {
			movePower.x = -(int)inputComponent.movePower;
			moveX = true;
		}

		if (itc[InputToggle::MoveDown]) {
			movePower.y = inputComponent.movePower;
			moveY = true;
		}

		if (itc[InputToggle::MoveRight]) {
			movePower.x = inputComponent.movePower;
			moveX = true;
		}

		if (moveX && moveY) {
			movePower.x *= factor;
			movePower.y *= factor;
		}

		forceComponent.x += movePower.x;
		forceComponent.y += movePower.y;
	}
}

ska::InputSystem::~InputSystem()
{
}
