#pragma once

#include "../ECS/ECSDefines.h"


namespace ska {
	class WorldCollisionComponent;
	class CollisionComponent;
	class CollidableComponent;

	struct CollisionEvent {
		CollisionEvent(const ska::EntityId e, WorldCollisionComponent* wcolComp, CollisionComponent* colComp, const CollidableComponent& colliComp) :
			entity(e), 
			collisionComponent(colComp), 
			wcollisionComponent(wcolComp),
			collidableComponent(colliComp) {
		}
		virtual ~CollisionEvent() = default;
		
		const ska::EntityId entity;
		CollisionComponent* collisionComponent;
		WorldCollisionComponent* wcollisionComponent;
		const CollidableComponent& collidableComponent;

	};
}