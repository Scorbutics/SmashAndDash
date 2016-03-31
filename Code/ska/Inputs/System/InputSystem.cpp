#include "InputSystem.h"

ska::InputSystem::InputSystem(const InputContextManager& icm, ska::EntityManager& entityManager) : System(entityManager), m_icm(icm) {
}

void ska::InputSystem::refresh() {
	for (EntityId entityId : m_processed) {
		InputComponent& inputComponent = m_entityManager.getComponent<InputComponent>(entityId);
		ForceComponent& forceComponent = m_entityManager.getComponent<ForceComponent>(entityId);
		PositionComponent& posComponent = m_entityManager.getComponent<PositionComponent>(entityId);

		Point<float> movePower;
		bool moveX = false;
		bool moveY = false;
		const float factor = 0.709;
		const InputToggleContainer& itc = m_icm.getToggles();
		const InputActionContainer& iac = m_icm.getActions();

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

		if (iac[InputAction::Jump] && posComponent.z < 0.001) {
			forceComponent.z += inputComponent.jumpPower;
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
