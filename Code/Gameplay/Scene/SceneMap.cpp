#include "../World/WorldScene.h"
#include "SceneMap.h"
#include "../CustomEntityManager.h"

#define MOB_SPAWNING_DELAY 5000

SceneMap::SceneMap(ska::Window& w, ska::SceneHolder& sh, PokemonGameEventDispatcher& ged, ska::InputContextManager& ril, WorldScene& ws, const std::string fileName, const std::string chipsetName, const bool sameMap) :
AbstractSceneMap_(w, ws, ged, sh, ril, sameMap),
m_fileName(fileName),
m_chipsetName(chipsetName),
m_iaRandomMovementSystem(ws.getEntityManager()),
m_iaDefinedMovementSystem(ws.getEntityManager(), &m_scriptAutoSystem),
m_mobSpawningSystem(ws, ws.getEntityManager(), MOB_SPAWNING_DELAY),
m_scriptAutoSystem(ws.getWorld(), ws.getEntityManager(), ws.getSaveGame(), ged),
m_scriptSystem(m_scriptAutoSystem, ril, ws.getWorld(), ws.getEntityManager()),
m_fightStartSystem(w, sh, ged, ws, ril, ws.getPlayer()),
m_cameraSystem(ws.getEntityManager(), ws.getScreenW(), ws.getScreenH()) {
	m_logics.push_back(&m_scriptSystem);
	m_logics.push_back(&m_iaRandomMovementSystem);
	m_logics.push_back(&m_iaDefinedMovementSystem);
	m_logics.push_back(&m_mobSpawningSystem);
	m_logics.push_back(&m_fightStartSystem);
	m_logics.push_back(&m_cameraSystem);

	m_scriptAutoSystem.addObserver(*this);
}

ska::CameraSystem& SceneMap::getCamera() {
	return m_cameraSystem;
}

SceneMap::SceneMap(ska::Window& w, PokemonGameEventDispatcher& ged, Scene& oldScene, WorldScene& ws, const std::string fileName, const std::string chipsetName, const bool sameMap) :
AbstractSceneMap_(w, ws, ged, oldScene, sameMap),
m_fileName(fileName),
m_chipsetName(chipsetName),
m_iaRandomMovementSystem(ws.getEntityManager()),
m_iaDefinedMovementSystem(ws.getEntityManager(), &m_scriptAutoSystem),
m_mobSpawningSystem(ws, ws.getEntityManager(), MOB_SPAWNING_DELAY),
m_scriptAutoSystem(ws.getWorld(), ws.getEntityManager(), ws.getSaveGame(), ged),
m_scriptSystem(m_scriptAutoSystem, m_inputCManager, ws.getWorld(), ws.getEntityManager()),
m_fightStartSystem(w, m_holder, ged, ws, m_inputCManager, ws.getPlayer()),
m_cameraSystem(ws.getEntityManager(), ws.getScreenW(), ws.getScreenH()) {
	m_logics.push_back(&m_scriptSystem);
	m_logics.push_back(&m_iaRandomMovementSystem);
	m_logics.push_back(&m_iaDefinedMovementSystem);
	m_logics.push_back(&m_mobSpawningSystem);
	m_logics.push_back(&m_fightStartSystem);
	m_logics.push_back(&m_cameraSystem);

	m_scriptAutoSystem.addObserver(*this);
}

SceneMap::SceneMap(ska::Window& w, ska::SceneHolder& sh, PokemonGameEventDispatcher& ged, ska::InputContextManager& ril, WorldScene& ws, const bool sameMap) :
SceneMap(w, sh, ged, ril, ws, ws.getSaveGame().getStartMapName(), ws.getSaveGame().getStartChipsetName(), sameMap) {

}

void SceneMap::load(ska::ScenePtr* lastScene) {
	AbstractSceneMap::load(lastScene);
	reinit();
	m_worldScene.load(lastScene);
}

bool SceneMap::unload() {
	m_worldScene.unload();
	return AbstractSceneMap::unload();
}

void SceneMap::reinit() {
	m_scriptSystem.clearNamedScriptedEntities();
	std::unordered_map<std::string, ska::EntityId> entities = m_worldScene.reinit(m_fileName, m_chipsetName);
	for (const auto& e : entities) {
		m_scriptSystem.registerNamedScriptedEntity(e.first, e.second);
	}

	/* Player Script ID = 0 */
	m_scriptSystem.registerNamedScriptedEntity("0", m_worldScene.getPlayer());
}


SceneMap::~SceneMap() {
	m_scriptAutoSystem.removeObserver(*this);
}
