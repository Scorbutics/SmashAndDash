#include "DebugAbstractSceneMap.h"
#include "../World/WorldScene.h"
#include "../CustomEntityManager.h"

DebugAbstractSceneMap::DebugAbstractSceneMap(ska::Window& w, WorldScene& ws, ska::GameEventDispatcher& ged, ska::SceneHolder& sh, ska::InputContextManager& ril, const bool sameMap) : AbstractSceneMap(w, ws, ged, sh, ril, sameMap),
m_debugWorldCollisionResponse(ged, ws.getEntityManager()),
m_debugCollisionDrawerSystem(ws.getEntityManager()) {
	m_logics.push_back(&m_debugCollisionDrawerSystem);
}

DebugAbstractSceneMap::DebugAbstractSceneMap(ska::Window& w, WorldScene& ws, ska::GameEventDispatcher& ged, ska::Scene& oldScene, const bool sameMap) : AbstractSceneMap(w, ws, ged, oldScene, sameMap),
m_debugWorldCollisionResponse(ged, ws.getEntityManager()),
m_debugCollisionDrawerSystem(ws.getEntityManager()) {

}
