#pragma once
#include "../../ska/ECS/System.h"
#include "../Component/MovementComponent.h"
#include "../../ska/Physic/PositionComponent.h"

class MovementSystem : public ska::System<ska::PositionComponent, MovementComponent> {
public:
	MovementSystem(ska::EntityManager& entityManager);
	virtual ~MovementSystem();
protected:
	virtual void refresh(ska::EntityId& entityId) override;
};

