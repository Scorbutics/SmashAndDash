#pragma once
#include "../Component/MovementComponent.h"
#include "../../ska/Physic/PositionComponent.h"
#include "../Component/HitboxComponent.h"
#include "../../ska/Graphic/Rectangle.h"
#include "../../ska/ECS/System.h"
class CollisionSystem : public ska::System<ska::PositionComponent, MovementComponent, HitboxComponent>
{
public:
	CollisionSystem(ska::EntityManager& entityManager);
	virtual ~CollisionSystem();
protected:
	virtual void refresh(ska::EntityId& entityId) override;
private:
	const ska::Rectangle createHitBox(ska::EntityId entityId);
};

