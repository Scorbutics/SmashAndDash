#pragma once
#include "AbstractStateMap.h"
#include "Graphic/System/DebugCollisionDrawerSystem.h"
#include "Physic/System/DebugWorldCollisionResponse.h"
#include "Data/Events/GameEventDispatcher.h"

namespace ska {
	class Window;
}

class DebugAbstractSceneMap : public AbstractStateMap {
public:
	DebugAbstractSceneMap(ska::Window& w, WorldState& ws, ska::GameEventDispatcher& ged, ska::StateHolder& sh, ska::InputContextManager& ril, const bool sameMap);
	DebugAbstractSceneMap(ska::Window& w, WorldState& ws, ska::GameEventDispatcher& ged, State& oldScene, const bool sameMap);
	virtual ~DebugAbstractSceneMap() = default;

private:
	ska::DebugCollisionDrawerSystem m_debugCollisionDrawerSystem;
	ska::DebugWorldCollisionResponse m_debugWorldCollisionResponse;
};
