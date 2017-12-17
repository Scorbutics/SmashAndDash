#include "AI/System/IARandomMovementSystem.h"
#include "AI/System/IADefinedMovementSystem.h"
#include "../Mobs/System/MobSpawningSystem.h"
#include "Script/System/ScriptRefreshSystem.h"
#include "../../Script/System/ScriptCommandsSystem.h"
#include "../Fight/System/FightStartSystem.h"
#include "Graphic/System/CameraFollowSystem.h"
#include "../World/WorldState.h"
#include "StateMap.h"

#define MOB_SPAWNING_DELAY 5000

StateMap::StateMap(CustomEntityManager& em, PokemonGameEventDispatcher& pged, WorldState& ws, const std::string& worldFileName, const std::string& worldChipsetName) :
	AbstractStateMap(em, pged),
	ska::SubObserver<ska::GameEvent>(std::bind(&StateMap::onGameEvent, this, std::placeholders::_1), pged),
	m_worldState(ws),
	m_fileName(worldFileName),
	m_chipsetName(worldChipsetName), 
	m_scriptAutoSystem(nullptr), 
	m_cameraSystem(nullptr), 
	m_scriptSystem(nullptr),
	m_worldCollisionResponse(ws.getWorld(), m_eventDispatcher, m_entityManager),
	m_entityCollisionResponse(m_eventDispatcher, m_entityManager){
	
}

bool StateMap::onGameEvent(ska::GameEvent& ge) {
	if (ge.getEventType() == ska::GameEventType::GAME_WINDOW_READY) {
        SKA_LOG_INFO("Game window is ready");
		m_cameraSystem = addLogic<ska::CameraFollowSystem>(ge.windowWidth, ge.windowHeight);
		m_worldState.linkCamera(m_cameraSystem);
		init();
	} else if (ge.getEventType() == ska::GameEventType::GAME_WINDOW_RESIZED) {
		m_cameraSystem->screenResized(ge.windowWidth, ge.windowHeight);
	}
	
	return true;
}

ska::CameraSystem& StateMap::getCamera() {
	return *m_cameraSystem;
}

void StateMap::init() {
    SKA_LOG_INFO("State Map initialization");
	m_scriptAutoSystem = addLogic<ScriptCommandsSystem>(m_worldState.getWorld(), m_worldState.getSaveGame(), m_eventDispatcher);
	addLogic<ska::IARandomMovementSystem>();
	addLogic<ska::IADefinedMovementSystem>(m_scriptAutoSystem);
	m_scriptSystem = addLogic<ska::ScriptRefreshSystem>(m_eventDispatcher, *m_scriptAutoSystem, m_worldState.getWorld(), m_worldState.getWorld());
	addLogic<MobSpawningSystem>(m_worldState, MOB_SPAWNING_DELAY);
	addLogic<FightStartSystem>(m_eventDispatcher, m_worldState, m_worldState.getPlayer());
	resetScriptEntities();
}

void StateMap::afterLoad(ska::StatePtr* lastScene) {
	AbstractStateMap::afterLoad(lastScene);
	SKA_LOG_INFO("State Map scripts reset");
	if(m_scriptSystem == nullptr) {
		return;
	}

	resetScriptEntities();
}

StateMap::~StateMap() {
}

void StateMap::resetScriptEntities() {
	m_scriptSystem->clearNamedScriptedEntities();
	auto entities = m_worldState.reinit(m_fileName, m_chipsetName);
	for (const auto& e : entities) {
		m_scriptSystem->registerNamedScriptedEntity(e.first, e.second);
	}

	/* Player Script ID = 0 */
	m_scriptSystem->registerNamedScriptedEntity("0", m_worldState.getPlayer());
}
