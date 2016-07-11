#include "../World/WorldScene.h"
#include "SceneMap.h"
#include "../CustomEntityManager.h"

#define MOB_SPAWNING_DELAY 5000

SceneMap::SceneMap(ska::SceneHolder& sh, ska::InputContextManager& ril, WorldScene& ws, const std::string fileName, const std::string chipsetName) :
AbstractSceneMap_(ws, sh, ril),
m_fileName(fileName),
m_chipsetName(chipsetName),
m_iaRandomMovementSystem(ws.getEntityManager()),
m_iaDefinedMovementSystem(ws.getEntityManager(), &m_scriptAutoSystem),
m_mobSpawningSystem(ws, ws.getEntityManager(), MOB_SPAWNING_DELAY),
m_scriptAutoSystem(ws.getEntityManager(), ws.getSaveGame()),
m_scriptSystem(m_scriptAutoSystem, ril, ws.getWorld(), ws.getEntityManager()),
m_fightStartSystem(sh, ws, ril, ws.getPlayer()),
m_cameraSystem(ws.getEntityManager(), ws.getScreenW(), ws.getScreenH()) {
	m_logics.push_back(&m_scriptSystem);
	m_logics.push_back(&m_iaRandomMovementSystem);
	m_logics.push_back(&m_iaDefinedMovementSystem);
	m_logics.push_back(&m_mobSpawningSystem);
	m_logics.push_back(&m_fightStartSystem);
	m_logics.push_back(&m_cameraSystem);	
}

ska::CameraSystem& SceneMap::getCamera() {
	return m_cameraSystem;
}

SceneMap::SceneMap(ska::Scene& oldScene, WorldScene& ws, const std::string fileName, const std::string chipsetName) :  
AbstractSceneMap_(ws, oldScene),
m_fileName(fileName),
m_chipsetName(chipsetName),
m_iaRandomMovementSystem(ws.getEntityManager()),
m_iaDefinedMovementSystem(ws.getEntityManager(), &m_scriptAutoSystem),
m_mobSpawningSystem(ws, ws.getEntityManager(), MOB_SPAWNING_DELAY),
m_scriptAutoSystem(ws.getEntityManager(), ws.getSaveGame()),
m_scriptSystem(m_scriptAutoSystem, m_inputCManager, ws.getWorld(), ws.getEntityManager()),
m_fightStartSystem(m_holder, ws, m_inputCManager, ws.getPlayer()),
m_cameraSystem(ws.getEntityManager(), ws.getScreenW(), ws.getScreenH()) {
	m_logics.push_back(&m_scriptSystem);
	m_logics.push_back(&m_iaRandomMovementSystem);
	m_logics.push_back(&m_iaDefinedMovementSystem);
	m_logics.push_back(&m_mobSpawningSystem);
	m_logics.push_back(&m_fightStartSystem);
	m_logics.push_back(&m_cameraSystem);
}

SceneMap::SceneMap(ska::SceneHolder& sh, ska::InputContextManager& ril, WorldScene& ws) : SceneMap(sh, ril, ws, ws.getSaveGame().getStartMapName(), ws.getSaveGame().getStartChipsetName()) {

}

void SceneMap::load(ska::ScenePtr* lastScene) {
	AbstractSceneMap::load(lastScene);
	reinit();
}

bool SceneMap::unload() {
	return AbstractSceneMap::unload();
}

void SceneMap::reinit() {
	m_scriptSystem.clearNamedScriptedEntities();
	std::unordered_map<std::string, ska::EntityId>& entities = m_worldScene.reinit(m_fileName, m_chipsetName);
	for (auto& e : entities) {
		m_scriptSystem.registerNamedScriptedEntity(e.first, e.second);
	}

	/* Player Script ID = 0 */
	m_scriptSystem.registerNamedScriptedEntity("0", m_worldScene.getPlayer());
}


SceneMap::~SceneMap()
{
}
