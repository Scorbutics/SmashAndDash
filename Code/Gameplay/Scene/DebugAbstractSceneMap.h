#pragma once
#include "AbstractSceneMap.h"
#include "../../ska/Graphic/System/DebugCollisionDrawerSystem.h"
#include "../../ska/Physic/System/DebugWorldCollisionResponse.h"

class DebugAbstractSceneMap : public AbstractSceneMap {
public:
	DebugAbstractSceneMap(WorldScene& ws, ska::SceneHolder& sh, ska::InputContextManager& ril, const bool sameMap);
	DebugAbstractSceneMap(WorldScene& ws, ska::Scene& oldScene, const bool sameMap);
	virtual ~DebugAbstractSceneMap() = default;

private:
	ska::DebugCollisionDrawerSystem m_debugCollisionDrawerSystem;
	ska::DebugWorldCollisionResponse m_debugWorldCollisionResponse;
};
