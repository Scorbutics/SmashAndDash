#pragma once
#include "../../ska/ECS/System.h"
#include "../Component/InputComponent.h"
#include "../Component/ForceComponent.h"
#include "../../ska/Inputs/InputListener.h"
#include "../../ska/Utils/Observer.h"

class InputSystem : public ska::System<InputComponent>, public ska::Observer<const int> {
public:
	InputSystem(ska::EntityManager& entityManager);
	virtual void update(ska::Observable<const int>* obs, const ska::EventArg& e, const int& t) override;
	virtual ~InputSystem();
protected:
	virtual void refresh(ska::EntityId& entityId) override;

};

