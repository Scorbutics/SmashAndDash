#include "DebugAbstractSceneMap.h"
#include "../World/WorldScene.h"
#include "../CustomEntityManager.h"

DebugAbstractSceneMap::DebugAbstractSceneMap(WorldScene& ws, ska::SceneHolder& sh, ska::InputContextManager& ril) : AbstractSceneMap(ws, sh, ril),
m_debugWorldCollisionResponse(m_collisionSystem, ws.getEntityManager()),
m_debugCollisionDrawerSystem(ws.getEntityManager()) {
	m_logics.push_back(&m_debugCollisionDrawerSystem);
}

DebugAbstractSceneMap::DebugAbstractSceneMap(WorldScene& ws, ska::Scene& oldScene) : AbstractSceneMap(ws, oldScene), 
m_debugWorldCollisionResponse(m_collisionSystem, ws.getEntityManager()),
m_debugCollisionDrawerSystem(ws.getEntityManager()) {

}
