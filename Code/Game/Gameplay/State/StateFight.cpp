#include "Graphic/System/CameraFixedStrategy.h"
#include "../World/WorldState.h"
#include "StateFight.h"
#include "StateGUIBattle.h"
#include "Task/Task.h"
#include "Task/CompoundTask.h"
#include "../Fight/FightComponent.h"
#include "../Data/PokemonDescriptor.h"
#include "Transition/Map/PokeballTransition.h"
#include "Transition/Map/DialogTransition.h"

#include "AI/IARandomMovementComponent.h"
#include "../Fight/System/SkillRefreshSystem.h"
#include "AI/System/IARandomMovementSystem.h"
#include "../Fight/System/BattleSystem.h"

StateFight::StateFight(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& ws, ska::Point<int> fightPos, FightComponent fc) :
	m_eventDispatcher(ged),
	m_entityManager(em),
	m_worldState(ws),
	m_opponentScriptId(fc.opponentScriptId),
	m_opponent("./Data/Monsters/" + ska::StringUtils::intToStr(fc.opponentScriptId) + ".ini"),
	m_pokemon("./Data/Monsters/" + ska::StringUtils::intToStr(fc.pokemonScriptId) + ".ini"),
	m_pokemonId(fc.fighterPokemon),
	m_trainerId(fc.trainer),
	m_opponentId(fc.fighterOpponent),
	m_sceneLoaded(false),
	m_loadState(0),
	m_ic(nullptr),
	m_skillFactory(ws, fc.level),
	m_loader(m_entityManager, m_eventDispatcher, m_worldState, m_pokemonId, m_opponentId, m_trainerId, m_pokeball, &m_ic, reinterpret_cast<ska::CameraSystem**>(&m_cameraSystem)),
	m_fightPos(fightPos) {
}

bool StateFight::onGameEvent(ska::GameEvent& ge) {
	if(ge.getEventType() == ska::GameEventType::GAME_WINDOW_READY ||
		ge.getEventType() == ska::GameEventType::GAME_WINDOW_RESIZED) {
		m_screenSize = ska::Point<int>(ge.windowWidth, ge.windowHeight);
	}
	return true;
}

void StateFight::beforeLoad(ska::State* lastScene) {
	/* If already loaded... */
	/*
	if (m_worldState.loadedOnce()) {
		m_entityManager.refreshEntities();
	}
	*/

	if (m_sceneLoaded) {
		return;
	}

	auto cameraSystem = std::make_unique<ska::CameraSystem>(m_entityManager, m_eventDispatcher, std::make_unique<ska::CameraFixedStrategy>(m_fightPos), m_screenSize.x, m_screenSize.y);
	m_cameraSystem = cameraSystem.get();
	addLogic(std::move(cameraSystem));	
	addLogic(std::make_unique<PokeballSystem>(m_entityManager));
	addLogic(std::make_unique<BattleSystem>(m_entityManager, m_pokemonId, m_opponentId, m_pokemon, m_opponent));
	addLogic(std::make_unique<SkillRefreshSystem>(m_entityManager));
	addLogic(std::make_unique<StatisticsSystem>(m_entityManager, m_worldState, m_eventDispatcher));
	addLogic(std::make_unique<ska::IARandomMovementSystem>(m_entityManager));

	addSubState(std::make_unique<StateGUIBattle>(m_entityManager, m_eventDispatcher));

	//TODO add IA input context ???
	//m_iaICM.addContext(ska::InputContextPtr());

	SkillsHolderComponent shc;
	m_skillFactory.loadSkills(m_pokemon, m_pokemonId, shc);
	m_entityManager.addComponent<SkillsHolderComponent>(m_pokemonId, std::move(shc));

	SkillsHolderComponent shcOpponent;
	m_skillFactory.loadSkills(m_opponent, m_opponentId, shcOpponent);

	m_entityManager.addComponent<SkillsHolderComponent>(m_opponentId, std::move(shcOpponent));
	m_descriptor.load(m_opponent, "Description");

	m_ic = &m_entityManager.getComponent<ska::InputComponent>(m_trainerId);
	m_entityManager.removeComponent<ska::InputComponent>(m_trainerId);


	auto delay = 3000;
	auto dialogTask = std::make_unique<DialogTransition>(delay, m_entityManager, m_eventDispatcher, m_trainerId, "Un " + m_descriptor.getName() + " sauvage apparaît !");
	auto pokeballTask = std::make_unique<PokeballTransition>(delay, m_entityManager, m_pokemonId, m_trainerId, m_pokeball);

	auto finalTask = m_loader.load();


	queueTask(std::move(dialogTask));
	queueTask(std::move(pokeballTask));
	queueTask(std::move(finalTask));
}

void StateFight::beforeUnload() {

	/* Triggers end fight cinematic to the next scene */
	const auto delay = 3000U;

	auto preTask = m_loader.unload();

    std::unique_ptr<ska::CompoundTask> firstTask;
    {
        auto pokeballTask = std::make_unique<PokeballTransition>(delay, m_entityManager, m_pokemonId, m_trainerId, m_pokeball);
        auto dialogTask = std::make_unique<DialogTransition>(delay, m_entityManager, m_eventDispatcher, m_trainerId, "Le " + m_descriptor.getName() + " a été battu.");
        firstTask = std::make_unique<ska::CompoundTask>(std::move(pokeballTask), std::move(dialogTask));
    }

	auto finalTask = std::make_unique<ska::Task>([&](ska::Task&) {
		m_loadState = 0;
		m_entityManager.removeComponent<ska::IARandomMovementComponent>(m_trainerId);
		m_entityManager.addComponent<ska::InputComponent>(m_trainerId, std::move(*m_ic));
		m_ic = nullptr;
		m_entityManager.removeEntity(m_pokemonId);
		return false;
	});


	queueTask(std::move(preTask));
	queueTask(std::move(firstTask));
	queueTask(std::move(finalTask));
	
}
