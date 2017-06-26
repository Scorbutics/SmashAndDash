/*#include "DebugAbstractSceneMap.h"
#include "../World/WorldState.h"

DebugAbstractSceneMap::DebugAbstractSceneMap(ska::Window& w, WorldState& ws, ska::GameEventDispatcher& ged, ska::StateHolder& sh, ska::InputContextManager& ril, const bool sameMap) : AbstractStateMap(w, ws, ged, sh, ril, sameMap),
m_debugCollisionDrawerSystem(ws.getEntityManager()),
m_debugWorldCollisionResponse(ged, ws.getEntityManager()) {

	m_logics.push_back(&m_debugCollisionDrawerSystem);
}

DebugAbstractSceneMap::DebugAbstractSceneMap(ska::Window& w, WorldState& ws, ska::GameEventDispatcher& ged, State& oldScene, const bool sameMap) : AbstractStateMap(w, ws, ged, oldScene, sameMap),
m_debugCollisionDrawerSystem(ws.getEntityManager()),
m_debugWorldCollisionResponse(ged, ws.getEntityManager()) {

}
*/