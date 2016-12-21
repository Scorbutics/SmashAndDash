#pragma once
#include "EventDispatcher.h"
#include "../Audio/SoundEvent.h"
#include "../Physic/CollisionEvent.h"
#include "../World/WorldEvent.h"

namespace ska {
	using GameEventDispatcher = ska::EventDispatcher<ska::SoundEvent, ska::WorldEvent, ska::CollisionEvent>;
}