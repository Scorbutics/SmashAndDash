#pragma once
#include "../../ska/ECS/System.h"
#include "../Component/GravityAffectedComponent.h"
#include "../Component/ForceComponent.h"
#include "../Component/MovementComponent.h"

class GravitySystem : public ska::System<ForceComponent, GravityAffectedComponent, MovementComponent> {
public:
	GravitySystem(ska::EntityManager& entityManager);
	virtual ~GravitySystem();
protected:
	virtual void refresh(ska::EntityId& entityId) override;
};

