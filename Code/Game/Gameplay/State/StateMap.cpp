#include "AI/System/IARandomMovementSystem.h"
#include "AI/System/IADefinedMovementSystem.h"
#include "../Mobs/System/MobSpawningSystem.h"
#include "Script/System/ScriptRefreshSystem.h"
#include "../../Script/System/ScriptCommandsSystem.h"
#include "../Fight/System/FightStartSystem.h"
#include "Graphic/System/CameraFollowSystem.h"
#include "../World/WorldState.h"
#include "../World/WorldStateChanger.h"
#include "StateMap.h"

#define MOB_SPAWNING_DELAY 5000

StateMap::StateMap(StateData& data, ska::StateHolder& sh, WorldStateChanger& wsc) :
	AbstractStateMap_(data, sh, wsc.worldState, wsc.sameMap), 
	ska::Observer<ska::GameEvent>(std::bind(&StateMap::onGameEvent, this, std::placeholders::_1)),
	m_fileName(wsc.worldFileName),
	m_chipsetName(wsc.worldChipsetName),
	m_worldCollisionResponse(wsc.worldState.getWorld(), data.m_eventDispatcher, m_entityManager),
	m_entityCollisionResponse(data.m_eventDispatcher, m_entityManager){
	
	m_cameraSystem = addLogic<ska::CameraFollowSystem>(wsc.screenSize.x, wsc.screenSize.y);
	init(data, wsc.worldState);
	m_eventDispatcher.ska::Observable<ska::GameEvent>::addObserver(*this);
}

StateMap::StateMap(StateData& data, State& oldScene, WorldStateChanger& wsc) :
	AbstractStateMap_(data, oldScene, wsc.worldState, wsc.sameMap),
	ska::Observer<ska::GameEvent>(std::bind(&StateMap::onGameEvent, this, std::placeholders::_1)),
	m_fileName(wsc.worldFileName),
	m_chipsetName(wsc.worldChipsetName),
	m_worldCollisionResponse(wsc.worldState.getWorld(), data.m_eventDispatcher, m_entityManager),
	m_entityCollisionResponse(data.m_eventDispatcher, m_entityManager) {
	
	m_cameraSystem = addLogic<ska::CameraFollowSystem>(wsc.screenSize.x, wsc.screenSize.y);
	init(data, wsc.worldState);
	m_eventDispatcher.ska::Observable<ska::GameEvent>::addObserver(*this);
}

bool StateMap::onGameEvent(ska::GameEvent& ge) {
	if (ge.getEventType() == ska::GameEventType::GAME_WINDOW_READY) {
		m_cameraSystem->screenResized(ge.windowWidth, ge.windowHeight);
	} else if (ge.getEventType() == ska::GameEventType::GAME_WINDOW_RESIZED) {
		m_cameraSystem->screenResized(ge.windowWidth, ge.windowHeight);
	}
	return true;
}

ska::CameraSystem& StateMap::getCamera() {
	return *m_cameraSystem;
}

void StateMap::init(StateData& data, WorldState& ws) {
	m_scriptAutoSystem = createLogic<ScriptCommandsSystem>(ws.getWorld(), ws.getSaveGame(), data.m_eventDispatcher);
	addLogic<ska::IARandomMovementSystem>();
	addLogic<ska::IADefinedMovementSystem>(m_scriptAutoSystem.get());
	m_scriptSystem = addLogic<ska::ScriptRefreshSystem>(m_eventDispatcher, *m_scriptAutoSystem, ws.getWorld(), ws.getWorld());
	addLogic<MobSpawningSystem>(ws, MOB_SPAWNING_DELAY);
	addLogic<FightStartSystem>(data.m_eventDispatcher, ws, ws.getPlayer());
	
}

void StateMap::afterLoad(ska::StatePtr* lastScene) {
	AbstractStateMap::afterLoad(lastScene);
	m_scriptSystem->clearNamedScriptedEntities();
	auto entities = m_worldState.reinit(m_fileName, m_chipsetName);
	for (const auto& e : entities) {
		m_scriptSystem->registerNamedScriptedEntity(e.first, e.second);
	}

	/* Player Script ID = 0 */
	m_scriptSystem->registerNamedScriptedEntity("0", m_worldState.getPlayer());
}

StateMap::~StateMap() {
	m_eventDispatcher.ska::Observable<ska::GameEvent>::addObserver(*this);
}
