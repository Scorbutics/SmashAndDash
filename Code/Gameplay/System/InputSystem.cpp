#include "InputSystem.h"

InputSystem::InputSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void InputSystem::refresh(ska::EntityId& entityId) {
	//InputComponent& inputComponent = m_entityManager.getComponent<InputComponent>(entityId);
	//ForceComponent& moveComponent = m_entityManager.getComponent<ForceComponent>(entityId);
	
}

void InputSystem::update(ska::Observable<const int>* obs, const ska::EventArg& e, const int& t) {

}

InputSystem::~InputSystem()
{
}
