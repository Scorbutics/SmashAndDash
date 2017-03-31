#pragma once
#include "EventDispatcher.h"
#include "SoundEvent.h"
#include "CollisionEvent.h"
#include "WorldEvent.h"

namespace ska {
	using GameEventDispatcher = EventDispatcher<SoundEvent, WorldEvent, CollisionEvent>;
}
