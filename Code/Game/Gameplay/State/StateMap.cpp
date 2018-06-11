#include "AI/System/IARandomMovementSystem.h"
#include "AI/System/IADefinedMovementSystem.h"
#include "../Mobs/System/MobSpawningSystem.h"
#include "Script/System/ScriptRefreshSystem.h"
#include "../../Script/System/ScriptCommandsSystem.h"
#include "../Fight/System/FightStartSystem.h"
#include "Graphic/System/CameraFollowStrategy.h"
#include "../World/WorldState.h"
#include "StateMap.h"
#include "Utils/FileUtils.h"
#include "World/System/ScriptWorldTriggerSystem.h"
#include "../CustomEntityManager.h"

#define MOB_SPAWNING_DELAY 5000

StateMap::StateMap(CustomEntityManager& em, PokemonGameEventDispatcher& pged, WorldState& ws, std::string worldFileName, std::string worldChipsetName, ska::Point<int> screenSize) :
	ska::SubObserver<ska::GameEvent>(std::bind(&StateMap::onGameEvent, this, std::placeholders::_1), pged),
	m_eventDispatcher(pged),
	m_entityManager(em),
	m_worldState(ws),
	m_fileName(std::move(worldFileName)),
	m_tilesetName(std::move(worldChipsetName)),
	m_scriptAutoSystem(nullptr),
	m_scriptSystem(nullptr),
	m_screenSize(screenSize) {

	const auto tilesetFnData = ska::FileNameData{ m_tilesetName };
	m_tilesetName = tilesetFnData.path + "/" + tilesetFnData.name;
}

bool StateMap::onGameEvent(ska::GameEvent& ge) {
	if (ge.getEventType() == ska::GameEventType::GAME_WINDOW_READY) {
        SKA_LOG_INFO("Game window is ready");
	}
	
	return true;
}

void StateMap::init() {
    SKA_LOG_INFO("State Map initialization");
	
	auto scriptAutoSys = std::make_unique<ScriptCommandsSystem>(m_entityManager, m_worldState.getWorld(), m_worldState.getSaveGame(), m_eventDispatcher);
	m_scriptAutoSystem = scriptAutoSys.get();
	addLogic(std::move(scriptAutoSys));

	addLogic(std::make_unique<ska::IARandomMovementSystem>(m_entityManager));
	addLogic(std::make_unique<ska::IADefinedMovementSystem>(m_worldState.getSpace(), m_entityManager, m_eventDispatcher));
	
	auto scriptSystem = std::make_unique<ska::ScriptRefreshSystem>(m_entityManager, m_eventDispatcher, *m_scriptAutoSystem, m_worldState.getWorld());
	m_scriptSystem = scriptSystem.get();
	addLogic(std::move(scriptSystem));
	addLogic(std::make_unique<ska::ScriptWorldTriggerSystem>(m_entityManager, m_eventDispatcher, m_worldState.getWorld()));
	addLogic(std::make_unique<MobSpawningSystem>(m_entityManager, m_worldState, MOB_SPAWNING_DELAY));
	addLogic(std::make_unique<FightStartSystem>(m_entityManager, m_eventDispatcher, m_worldState, m_worldState.getPlayer()));
}

void StateMap::beforeLoad(ska::State* lastState) {
	init();
	
}

void StateMap::afterLoad(ska::State * lastState){
	
}