#pragma once

#include "../ECS/ECSDefines.h"

namespace ska {
	struct CollisionEvent {
		CollisionEvent(const ska::EntityId e) : entity(e) {
		}
		virtual ~CollisionEvent() = default;
		
		const ska::EntityId entity;
	};
}