#include "InputSystem.h"

ska::InputSystem::InputSystem(EntityManager& entityManager, const InputContextManager& icm) : System(entityManager), m_icm(icm) {
}

void ska::InputSystem::refresh() {
	for (EntityId entityId : m_processed) {
		InputComponent& inputComponent = m_entityManager.getComponent<InputComponent>(entityId);
		ForceComponent& forceComponent = m_entityManager.getComponent<ForceComponent>(entityId);
		PositionComponent& posComponent = m_entityManager.getComponent<PositionComponent>(entityId);

		Point<float> movePower;
		bool moveX = false;
		bool moveY = false;
		const float factor = 0.709F;
		const InputToggleContainer& itc = m_icm.getToggles();
		const InputActionContainer& iac = m_icm.getActions();

		if (itc[MoveUp]) {
			movePower.y = -static_cast<float>(inputComponent.movePower);
			moveY = true;
		}

		if (itc[MoveLeft]) {
			movePower.x = -static_cast<float>(inputComponent.movePower);
			moveX = true;
		}

		if (itc[MoveDown]) {
			movePower.y = static_cast<float>(inputComponent.movePower);
			moveY = true;
		}

		if (itc[MoveRight]) {
			movePower.x = static_cast<float>(inputComponent.movePower);
			moveX = true;
		}

		if (iac[Jump] && posComponent.z < 0.001) {
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
