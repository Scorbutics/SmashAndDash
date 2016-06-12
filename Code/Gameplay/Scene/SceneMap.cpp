#include "../World/WorldScene.h"
#include "SceneMap.h"
#include "../CustomEntityManager.h"

#define MOB_SPAWNING_DELAY 5000

SceneMap::SceneMap(ska::SceneHolder& sh, ska::InputContextManager& ril, WorldScene& ws, const std::string fileName, const std::string chipsetName) :
AbstractSceneMap(sh, ril),
m_fileName(fileName),
m_chipsetName(chipsetName),
m_worldScene(ws),
m_iaMovementSystem(ws.getEntityManager()),
m_mobSpawningSystem(ws, ws.getEntityManager(), MOB_SPAWNING_DELAY),
m_scriptAutoSystem(ws.getEntityManager(), ws.getSaveGame()),
m_scriptSystem(m_scriptAutoSystem, ril, ws.getWorld().getBlockSize(), ws.getEntityManager()),
m_fightStartSystem(sh, ws, ril, ws.getPlayer()),
m_cameraSystem(ws.getEntityManager(), ws.getScreenW(), ws.getScreenH()),
m_collisionSystem(ws.getWorld(), ws.getEntityManager()) {
	m_logics.push_back(&m_scriptSystem);
	m_logics.push_back(&m_iaMovementSystem);
	m_logics.push_back(&m_mobSpawningSystem);
	m_logics.push_back(&m_fightStartSystem);
	m_logics.push_back(&m_cameraSystem);	
	m_logics.push_back(&m_collisionSystem);
}

SceneMap::SceneMap(ska::Scene& oldScene, WorldScene& ws, const std::string fileName, const std::string chipsetName) :  
AbstractSceneMap(oldScene),
m_fileName(fileName),
m_chipsetName(chipsetName),
m_worldScene(ws),
m_iaMovementSystem(ws.getEntityManager()),
m_mobSpawningSystem(ws, ws.getEntityManager(), MOB_SPAWNING_DELAY),
m_scriptAutoSystem(ws.getEntityManager(), ws.getSaveGame()),
m_scriptSystem(m_scriptAutoSystem, m_inputCManager, ws.getWorld().getBlockSize(), ws.getEntityManager()),
m_fightStartSystem(m_holder, ws, m_inputCManager, ws.getPlayer()),
m_cameraSystem(ws.getEntityManager(), ws.getScreenW(), ws.getScreenH()), 
m_collisionSystem(ws.getWorld(), ws.getEntityManager()) {
	m_logics.push_back(&m_scriptSystem);
	m_logics.push_back(&m_iaMovementSystem);
	m_logics.push_back(&m_mobSpawningSystem);
	m_logics.push_back(&m_fightStartSystem);
	m_logics.push_back(&m_cameraSystem);
	m_logics.push_back(&m_collisionSystem);
}

SceneMap::SceneMap(ska::SceneHolder& sh, ska::InputContextManager& ril, WorldScene& ws) : SceneMap(sh, ril, ws, ws.getSaveGame().getStartMapName(), ws.getSaveGame().getStartChipsetName()) {

}

void SceneMap::graphicUpdate(ska::DrawableContainer& drawables) {
	m_worldScene.graphicUpdate(drawables);
	AbstractSceneMap::graphicUpdate(drawables);
}

void SceneMap::load() {
	m_worldScene.linkCamera(&m_cameraSystem);
	reinit();
}

void SceneMap::unload() {
	m_worldScene.unload();
}

void SceneMap::reinit() {
	bool firstTime = !m_worldScene.loadedOnce();
	if (!firstTime) {
		/* Do not delete the player between 2 maps, just TP it */
		std::unordered_set<ska::EntityId> toNotDelete;
		toNotDelete.insert(m_worldScene.getPlayer());

		/* Delete others entities */
		m_worldScene.getEntityManager().removeEntities(toNotDelete);
	}

	m_scriptSystem.clearNamedScriptedEntities();
	std::unordered_map<std::string, ska::EntityId>& entities = m_worldScene.reinit(m_fileName, m_chipsetName);
	for (auto& e : entities) {
		m_scriptSystem.registerNamedScriptedEntity(e.first, e.second);
	}

	/* Player Script ID = 0 */
	m_scriptSystem.registerNamedScriptedEntity("0", m_worldScene.getPlayer());
}

void SceneMap::eventUpdate(bool stuck) {
	AbstractSceneMap::eventUpdate(stuck);
	m_worldScene.eventUpdate(stuck);
}

SceneMap::~SceneMap()
{
}
