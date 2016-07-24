#include "DebugAbstractSceneMap.h"
#include "../World/WorldScene.h"
#include "../CustomEntityManager.h"

DebugAbstractSceneMap::DebugAbstractSceneMap(WorldScene& ws, ska::SceneHolder& sh, ska::InputContextManager& ril, const bool sameMap) : AbstractSceneMap(ws, sh, ril, sameMap),
m_debugWorldCollisionResponse(m_collisionSystem, ws.getEntityManager()),
m_debugCollisionDrawerSystem(ws.getEntityManager()) {
	m_logics.push_back(&m_debugCollisionDrawerSystem);
}

DebugAbstractSceneMap::DebugAbstractSceneMap(WorldScene& ws, ska::Scene& oldScene, const bool sameMap) : AbstractSceneMap(ws, oldScene, sameMap),
m_debugWorldCollisionResponse(m_collisionSystem, ws.getEntityManager()),
m_debugCollisionDrawerSystem(ws.getEntityManager()) {

}
