#include "Game/__internalConfig/LoggerConfig.h"
#include "AI/System/IARandomMovementSystem.h"
#include "AI/System/IADefinedMovementSystem.h"
#include "AI/System/IAFollowSystem.h"
#include "../Control/GridMovementSystem.h"
#include "../Mobs/System/MobSpawningSystem.h"
#include "Script/System/ScriptRefreshSystem.h"
#include "../Fight/System/FightStartSystem.h"
#include "../Mobs/System/ArrivalSystem.h"
#include "Graphic/System/CameraFollowStrategy.h"
#include "../World/WorldState.h"
#include "StateMap.h"
#include "Base/IO/Files/FileUtils.h"
#include "World/System/ScriptWorldTriggerSystem.h"
#include "../CustomEntityManager.h"
#include "../../Script/ScriptConstants.h"
#include "Core/ECS/Basics/Script/ScriptAwareComponent.h"
#include "Physic/Space.h"
#include <chipmunk/chipmunk.h>

#define MOB_SPAWNING_DELAY 5000

StateMap::StateMap(CustomEntityManager& em, PokemonGameEventDispatcher& pged, WorldState& ws, std::string worldFileName, std::string worldChipsetName, std::optional<ska::Point<int>> trainerPosition, bool walkIn, bool resume) :
	ska::SubObserver<ska::GameEvent>(std::bind(&StateMap::onGameEvent, this, std::placeholders::_1), pged),
	m_eventDispatcher(pged),
	m_entityManager(em),
	m_worldState(ws),
	m_fileName(std::move(worldFileName)),
	m_tilesetName(std::move(worldChipsetName)),
	m_trainerPosition(trainerPosition),
	m_walkIn(walkIn),
	m_resume(resume) {

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
	addLogic(std::make_unique<ska::IAFollowSystem>(m_entityManager));
	// The player moves block by block on the map
	addLogic(std::make_unique<GridMovementSystem>(m_entityManager, m_eventDispatcher, m_worldState));
	m_itemShortcuts = std::make_unique<ItemShortcuts>(m_eventDispatcher, m_worldState);
	m_inputHooks = std::make_unique<InputHooks>(m_eventDispatcher, m_worldState);

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
	// The wild Pokemon arriving (flying in, out of a hideout) or flying away
	auto arrivals = std::make_unique<ArrivalSystem>(m_entityManager, [this](const ArrivalComponent& arrival) { m_worldState.wildArrived(arrival); });
	const auto* arrivalSystem = arrivals.get();
	addLogic(std::move(arrivals));
	m_wildMarks = std::make_unique<WildPokemonMarks>(m_entityManager, m_worldState.getCamera(), arrivalSystem);
}

void StateMap::onGraphicUpdate(unsigned int, ska::DrawableContainer& drawables) {
	if (m_wildMarks != nullptr) {
		m_wildMarks->graphicUpdate(drawables);
	}
}

void StateMap::afterLoad(ska::State * lastState){
	// Back from a battle : the map goes on as it is (its characters, its wild Pokemon, its scripts)
	if (!m_resume) {
		m_worldState.reinit(m_fileName, m_tilesetName);
	}
	m_worldState.getCamera().setZoom(WorldState::MapZoom);
	// The player moves block by block on the map (GridMovementSystem)
	if (auto* trainer = m_worldState.getEntityLocator().getEntityId(SCRIPT_ENTITY_TRAINER); trainer != nullptr) {
		// A new map : no step in progress (ex : a step interrupted by a cutscene that teleported the player)
		auto grid = GridMovementComponent{};
		if (m_entityManager.hasComponent<GridMovementComponent>(*trainer)) {
			grid.facing = m_entityManager.getComponent<GridMovementComponent>(*trainer).facing;
		}
		// Back from a battle : it faces where it threw its pokeball
		if (m_resume && m_entityManager.hasComponent<ska::AnimationComponent>(*trainer)) {
			grid.facing = m_entityManager.getComponent<ska::AnimationComponent>(*trainer).state;
		}
		m_entityManager.addComponent<GridMovementComponent>(*trainer, std::move(grid));
	}
	// The interface of the map shows the bag
	m_worldState.getBag().notify();
	if (m_trainerPosition.has_value()) {
		auto* trainer = m_worldState.getEntityLocator().getEntityId(SCRIPT_ENTITY_TRAINER);
		if (trainer != nullptr) {
			auto& position = m_entityManager.getComponent<ska::PositionComponent>(*trainer);
			position.x = static_cast<float>(m_trainerPosition->x);
			position.y = static_cast<float>(m_trainerPosition->y);
			if (m_walkIn) {
				walkIn(*trainer);
			}
		}
	}
}

void StateMap::walkIn(ska::EntityId trainer) {
	// As the classic games : the player appears on the door (the block behind its position, from the direction it faces)
	// and walks out of it while the screen lightens
	auto& grid = m_entityManager.getComponent<GridMovementComponent>(trainer);
	auto& position = m_entityManager.getComponent<ska::PositionComponent>(trainer);
	const auto& world = m_worldState.getWorld();
	const auto blockSize = static_cast<int>(world.getBlockSize());
	const int behindX[] = { 0, -1, 0, 1 };
	const int behindY[] = { -1, 0, 1, 0 };
	const auto direction = grid.facing >= 0 && grid.facing < 4 ? grid.facing : 0;
	const auto x = m_trainerPosition->x / blockSize + behindX[direction];
	const auto y = m_trainerPosition->y / blockSize + behindY[direction];
	if (x < 0 || y < 0 || static_cast<std::size_t>(x) >= world.getBlocksX() || static_cast<std::size_t>(y) >= world.getBlocksY()) {
		return;
	}
	// A wall behind (ex : a map edge from another direction) : it appears on its position
	const auto margin = 4.;
	const auto box = cpBBNew(x * blockSize + margin, y * blockSize + margin, (x + 1) * blockSize - margin, (y + 1) * blockSize - margin);
	if (m_worldState.getSpace().staticShapeIn(box)) {
		return;
	}
	position.x = static_cast<float>(x * blockSize);
	position.y = static_cast<float>(y * blockSize);
	grid.forcedSteps = 1;
	grid.forcedDirection = direction;
	grid.forcedDuration = GridMovementSystem::WalkStep;
	// The door is already entered : its scripts (the teleport back) do not run
	if (m_entityManager.hasComponent<ska::ScriptAwareComponent>(trainer)) {
		m_entityManager.getComponent<ska::ScriptAwareComponent>(trainer).enteredBlock = ska::Point<int>{ x, y };
	}
}
