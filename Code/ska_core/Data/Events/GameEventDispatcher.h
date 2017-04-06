#pragma once
#include "EventDispatcher.h"
#include "SoundEvent.h"
#include "CollisionEvent.h"
#include "WorldEvent.h"
#include "GUIEvent.h"

namespace ska {
	using GameEventDispatcher = EventDispatcher<SoundEvent, WorldEvent, CollisionEvent, GUIEvent>;
}
