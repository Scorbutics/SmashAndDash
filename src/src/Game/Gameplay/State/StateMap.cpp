#include "Game/__internalConfig/LoggerConfig.h"
#include "AI/System/IARandomMovementSystem.h"
#include "AI/System/IADefinedMovementSystem.h"
#include "../Mobs/System/MobSpawningSystem.h"
#include "Script/System/ScriptRefreshSystem.h"
#include "../Fight/System/FightStartSystem.h"
#include "Graphic/System/CameraFollowStrategy.h"
#include "../World/WorldState.h"
#include "StateMap.h"
#include "Base/IO/Files/FileUtils.h"
#include "World/System/ScriptWorldTriggerSystem.h"
#include "../CustomEntityManager.h"
#include "../../Script/ScriptConstants.h"

#define MOB_SPAWNING_DELAY 5000

StateMap::StateMap(CustomEntityManager& em, PokemonGameEventDispatcher& pged, WorldState& ws, std::string worldFileName, std::string worldChipsetName, std::optional<ska::Point<int>> trainerPosition) :
	ska::SubObserver<ska::GameEvent>(std::bind(&StateMap::onGameEvent, this, std::placeholders::_1), pged),
	m_eventDispatcher(pged),
	m_entityManager(em),
	m_worldState(ws),
	m_fileName(std::move(worldFileName)),
	m_tilesetName(std::move(worldChipsetName)),
	m_trainerPosition(trainerPosition) {

	const auto tilesetFnData = ska::FileNameData{ m_tilesetName };
	m_tilesetName = tilesetFnData.path + "/" + tilesetFnData.name;
}

bool StateMap::onGameEvent(ska::GameEvent& ge) {
	if (ge.getEventType() == ska::GameEventType::GAME_WINDOW_READY) {
		SLOG(ska::LogLevel::Info) << "Game window is ready";
	}
	
	return true;
}

void StateMap::beforeLoad(ska::State* lastState) {
	SLOG(ska::LogLevel::Info) << "State Map initialization";

	addLogic(std::make_unique<ska::IARandomMovementSystem>(m_entityManager));
	addLogic(std::make_unique<ska::IADefinedMovementSystem>(m_worldState.getSpace(), m_entityManager, m_eventDispatcher));

	// The map loading scripts (ex : the characters of the map) start before the positioned ones (ex : a script on the block
	// where the player arrives)
	addLogic(std::make_unique<ska::ScriptWorldTriggerSystem>(m_entityManager, m_eventDispatcher, m_worldState.getWorld()));
	auto scriptSystem = std::make_unique<ska::ScriptRefreshSystem>(m_entityManager, m_eventDispatcher, m_worldState.getWorld());
	m_scriptSystem = scriptSystem.get();
	// The scripts are skalang scripts
	m_scriptSystem->addScriptRunner(".miniska", m_worldState.getScriptRunner());
	addLogic(std::move(scriptSystem));
	addLogic(std::make_unique<MobSpawningSystem>(m_entityManager, m_worldState, MOB_SPAWNING_DELAY));
	addLogic(std::make_unique<FightStartSystem>(m_entityManager, m_eventDispatcher, m_worldState, m_worldState.getEntityLocator()));
}

void StateMap::afterLoad(ska::State * lastState){
	m_worldState.reinit(m_fileName, m_tilesetName);
	if (m_trainerPosition.has_value()) {
		auto* trainer = m_worldState.getEntityLocator().getEntityId(SCRIPT_ENTITY_TRAINER);
		if (trainer != nullptr) {
			auto& position = m_entityManager.getComponent<ska::PositionComponent>(*trainer);
			position.x = static_cast<float>(m_trainerPosition->x);
			position.y = static_cast<float>(m_trainerPosition->y);
		}
	}
}