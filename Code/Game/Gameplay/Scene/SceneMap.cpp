#include "AI/System/IARandomMovementSystem.h"
#include "AI/System/IADefinedMovementSystem.h"
#include "../Mobs/System/MobSpawningSystem.h"
#include "Script/System/ScriptRefreshSystem.h"
#include "../../Script/System/ScriptCommandsSystem.h"
#include "../Fight/System/FightStartSystem.h"
#include "Graphic/System/CameraFollowSystem.h"


#include "Core/Window.h"
#include "../World/WorldScene.h"
#include "SceneMap.h"
#include "../CustomEntityManager.h"

#define MOB_SPAWNING_DELAY 5000

SceneMap::SceneMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::SceneHolder& sh, WorldScene& ws, const std::string fileName, const std::string chipsetName, const bool sameMap) :
AbstractSceneMap_(em, ged, w, ril, sh, ws, sameMap),
m_fileName(fileName),
m_chipsetName(chipsetName) {
	
	m_scriptAutoSystem = createLogic<ScriptCommandsSystem>(ws.getWorld(), ws.getSaveGame(), ged);
	addLogic<ska::IARandomMovementSystem>();
	addLogic<ska::IADefinedMovementSystem>(m_scriptAutoSystem.get());
	m_scriptSystem = addLogic<ska::ScriptRefreshSystem>(*m_scriptAutoSystem, ril, ws.getWorld(), ws.getWorld());
	addLogic<MobSpawningSystem>(ws, MOB_SPAWNING_DELAY);
	addLogic<FightStartSystem>(w, ged, ws, ril, ws.getPlayer());
	m_cameraSystem = addLogic<ska::CameraFollowSystem>(m_window.getWidth(), m_window.getHeight());
}

ska::CameraSystem& SceneMap::getCamera() {
	return *m_cameraSystem;
}

SceneMap::SceneMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, Scene& oldScene, WorldScene& ws, const std::string fileName, const std::string chipsetName, const bool sameMap) :
AbstractSceneMap_(em, ged, w, ril, oldScene, ws, sameMap),
m_fileName(fileName),
m_chipsetName(chipsetName) {

	m_scriptAutoSystem = createLogic<ScriptCommandsSystem>(ws.getWorld(), ws.getSaveGame(), ged);
	addLogic<ska::IARandomMovementSystem>();
	addLogic<ska::IADefinedMovementSystem>(m_scriptAutoSystem.get());
	m_scriptSystem = addLogic<ska::ScriptRefreshSystem>(*m_scriptAutoSystem, ril, ws.getWorld(), ws.getWorld());
	addLogic<MobSpawningSystem>(ws, MOB_SPAWNING_DELAY);
	addLogic<FightStartSystem>(w, ged, ws, ril, ws.getPlayer());
	m_cameraSystem = addLogic<ska::CameraFollowSystem>(m_window.getWidth(), m_window.getHeight());
}

void SceneMap::afterLoad(ska::ScenePtr* lastScene) {
	AbstractSceneMap::afterLoad(lastScene);
	m_scriptSystem->clearNamedScriptedEntities();
	auto entities = m_worldScene.reinit(m_fileName, m_chipsetName);
	for (const auto& e : entities) {
		m_scriptSystem->registerNamedScriptedEntity(e.first, e.second);
	}

	/* Player Script ID = 0 */
	m_scriptSystem->registerNamedScriptedEntity("0", m_worldScene.getPlayer());
}

SceneMap::~SceneMap() {
}
