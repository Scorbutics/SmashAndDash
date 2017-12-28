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

StateMap::StateMap(CustomEntityManager& em, PokemonGameEventDispatcher& pged, WorldState& ws, const std::string& worldFileName, const std::string& worldChipsetName, ska::Point<int> screenSize) :
	AbstractStateMap(em, pged, ws),
	ska::SubObserver<ska::GameEvent>(std::bind(&StateMap::onGameEvent, this, std::placeholders::_1), pged),
	m_worldState(ws),
	m_fileName(worldFileName),
	m_chipsetName(worldChipsetName), 
	m_scriptAutoSystem(nullptr), 
	m_cameraSystem(nullptr), 
	m_scriptSystem(nullptr),
	m_worldCollisionResponse(ws.getWorld(), m_eventDispatcher, m_entityManager),
	m_entityCollisionResponse(m_eventDispatcher, m_entityManager),
	m_screenSize(screenSize) {

}

bool StateMap::onGameEvent(ska::GameEvent& ge) {
	if (ge.getEventType() == ska::GameEventType::GAME_WINDOW_READY) {
        SKA_LOG_INFO("Game window is ready");
	}
	
	return true;
}

ska::CameraSystem* StateMap::getCamera() {
	return m_cameraSystem;
}

void StateMap::init() {
    SKA_LOG_INFO("State Map initialization");
	auto cameraSystem = std::make_unique<ska::CameraFollowSystem>(m_entityManager, m_eventDispatcher, m_screenSize.x, m_screenSize.y);
	m_cameraSystem = cameraSystem.get();
	addLogic(std::move(cameraSystem));
	
	auto scriptAutoSys = std::make_unique<ScriptCommandsSystem>(m_entityManager, m_worldState.getWorld(), m_worldState.getSaveGame(), m_eventDispatcher);
	m_scriptAutoSystem = scriptAutoSys.get();
	addLogic(std::move(scriptAutoSys));

	addLogic(std::make_unique<ska::IARandomMovementSystem>(m_entityManager));
	addLogic(std::make_unique<ska::IADefinedMovementSystem>(m_entityManager, m_scriptAutoSystem));
	
	auto scriptSystem = std::make_unique<ska::ScriptRefreshSystem>(m_entityManager, m_eventDispatcher, *m_scriptAutoSystem, m_worldState.getWorld(), m_worldState.getWorld());
	m_scriptSystem = scriptSystem.get();
	addLogic(std::move(scriptSystem));
	addLogic(std::make_unique<MobSpawningSystem>(m_entityManager, m_worldState, MOB_SPAWNING_DELAY));
	addLogic(std::make_unique<FightStartSystem>(m_entityManager, m_eventDispatcher, m_worldState, m_worldState.getPlayer()));
	//resetScriptEntities();
}

void StateMap::beforeLoad(ska::State* lastScene) {
	AbstractStateMap::beforeLoad(lastScene);
	
	// Do not delete the player between 2 maps, just TP it
	std::unordered_set<ska::EntityId> toNotDelete;
	toNotDelete.insert(m_worldState.getPlayer());

	// If the map changes, we delete all entities (except player)
	m_entityManager.removeEntities(toNotDelete);

	init();
	resetScriptEntities();
}

StateMap::~StateMap() {
}

void StateMap::resetScriptEntities() {
	SKA_LOG_INFO("State Map scripts reset");
	m_scriptSystem->clearNamedScriptedEntities();
	auto entities = m_worldState.reinit(m_fileName, m_chipsetName);
	for (const auto& e : entities) {
		m_scriptSystem->registerNamedScriptedEntity(e.first, e.second);
	}

	/* Player Script ID = 0 */
	m_scriptSystem->registerNamedScriptedEntity("0", m_worldState.getPlayer());
}
