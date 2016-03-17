#include "InputSystem.h"

ska::InputSystem::InputSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void ska::InputSystem::refresh() {
	//InputComponent& inputComponent = m_entityManager.getComponent<InputComponent>(entityId);
	//ForceComponent& moveComponent = m_entityManager.getComponent<ForceComponent>(entityId);
	
}

void ska::InputSystem::update(ska::Observable<const int>* obs, const ska::EventArg& e, const int& t) {

}

ska::InputSystem::~InputSystem()
{
}
