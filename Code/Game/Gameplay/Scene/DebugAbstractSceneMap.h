#pragma once
#include "AbstractSceneMap.h"
#include "Graphic/System/DebugCollisionDrawerSystem.h"
#include "Physic/System/DebugWorldCollisionResponse.h"
#include "Data/GameEventDispatcher.h"

namespace ska {
	class Window;
}

class DebugAbstractSceneMap : public AbstractSceneMap {
public:
	DebugAbstractSceneMap(ska::Window& w, WorldScene& ws, ska::GameEventDispatcher& ged, ska::SceneHolder& sh, ska::InputContextManager& ril, const bool sameMap);
	DebugAbstractSceneMap(ska::Window& w, WorldScene& ws, ska::GameEventDispatcher& ged, Scene& oldScene, const bool sameMap);
	virtual ~DebugAbstractSceneMap() = default;

private:
	ska::DebugCollisionDrawerSystem m_debugCollisionDrawerSystem;
	ska::DebugWorldCollisionResponse m_debugWorldCollisionResponse;
};
