#include "InputSystem.h"
#include "../../Data/Events/InputKeyEvent.h"

ska::InputSystem::InputSystem(EntityManager& entityManager, GameEventDispatcher& ged) :
	System(entityManager),
	ska::Observer<InputKeyEvent>(std::bind(&ska::InputSystem::onKeyInputEvent, this, std::placeholders::_1)),
	m_eventDispatcher(ged) {
	m_eventDispatcher.ska::Observable<InputKeyEvent>::addObserver(*this);
}

void ska::InputSystem::refresh(unsigned int) {
}

bool ska::InputSystem::onKeyInputEvent(InputKeyEvent& ike) {
	const auto& icm = ike.icm;
	const auto& processed = getEntities();
	for (const auto& entityId : processed) {
		auto& inputComponent = m_componentAccessor.get<InputComponent>(entityId);
		auto& forceComponent = m_componentAccessor.get<ForceComponent>(entityId);
		auto& posComponent = m_componentAccessor.get<PositionComponent>(entityId);

		Point<float> movePower;
		auto moveX = false;
		auto moveY = false;
		const auto factor = 0.717F;
		const auto& itc = icm.getToggles();
		const auto& iac = icm.getActions();

		if (itc[MoveUp]) {
			movePower.y = -inputComponent.movePower;
			moveY = true;
		}

		if (itc[MoveLeft]) {
			movePower.x = -inputComponent.movePower;
			moveX = true;
		}

		if (itc[MoveDown]) {
			movePower.y = inputComponent.movePower;
			moveY = true;
		}

		if (itc[MoveRight]) {
			movePower.x = inputComponent.movePower;
			moveX = true;
		}

		if (iac[Jump] && posComponent.z < 0.001) {
			//Jump is an "impulse", not really a force, so it's more directly applied (we divide it by dt)
			forceComponent.z += inputComponent.jumpPower;
		}

		if (moveX && moveY) {
			movePower.x *= factor;
			movePower.y *= factor;
		}

		forceComponent.x += movePower.x;
		forceComponent.y += movePower.y;
	}
	return true;
}

ska::InputSystem::~InputSystem() {
	m_eventDispatcher.ska::Observable<InputKeyEvent>::removeObserver(*this);
}
