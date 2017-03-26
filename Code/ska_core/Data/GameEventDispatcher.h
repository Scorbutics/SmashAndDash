#pragma once
#include "EventDispatcher.h"
#include "Data/Events/SoundEvent.h"
#include "Data/Events/CollisionEvent.h"
#include "Data/Events/WorldEvent.h"

namespace ska {
	using GameEventDispatcher = EventDispatcher<SoundEvent, WorldEvent, CollisionEvent>;
}
