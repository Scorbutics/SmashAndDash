#pragma once
#include "../../ska/ECS/System.h"
#include "../Component/MovementComponent.h"
#include "../Component/ForceComponent.h"
class ForceSystem : public ska::System<ForceComponent, MovementComponent> {
public:
	ForceSystem(ska::EntityManager& entityManager);
	virtual ~ForceSystem();
protected:
	virtual void refresh(ska::EntityId& entityId) override;
};

