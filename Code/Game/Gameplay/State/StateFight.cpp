#include "Core/Window.h"
#include "../World/WorldState.h"
#include "StateFight.h"
#include "StateGUIBattle.h"
#include "Task/Task.h"
#include "Task/CompoundTask.h"
#include "../CustomEntityManager.h"
#include "../Fight/FightComponent.h"
#include "../Data/PokemonDescriptor.h"
#include "Transition/Map/PokeballTransition.h"
#include "Transition/Map/DialogTransition.h"

#include "../PokemonGameEventDispatcher.h"
#include "AI/IARandomMovementComponent.h"
#include "Utils/ununique.h"
#include "../Fight/System/SkillRefreshSystem.h"
#include "AI/System/IARandomMovementSystem.h"

StateFight::StateFight(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::State& oldScene, WorldState& ws, ska::Point<int> fightPos, FightComponent fc) :
AbstractStateMap_(em, ged, w, ril, oldScene, ws, true),
m_iaICM(ska::InputContextManager::instantiateEmpty(ril)),
m_opponentScriptId(fc.opponentScriptId),
m_skillFactory(ws, fc.level),
m_opponent("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Monsters" FILE_SEPARATOR + ska::StringUtils::intToStr(fc.opponentScriptId) + ".ini"),
m_pokemon("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Monsters" FILE_SEPARATOR + ska::StringUtils::intToStr(fc.pokemonScriptId) + ".ini"),
m_pokemonId(fc.fighterPokemon),
m_trainerId(fc.trainer),
m_opponentId(fc.fighterOpponent),
m_sceneLoaded(false),
m_loadState(0),
m_worldEntityCollisionResponse(ws.getWorld(), ged, m_entityManager),
m_skillEntityCollisionResponse(*m_collisionSystem, ged, m_entityManager),
m_ic(nullptr) {

	m_cameraSystem = addLogic<ska::CameraFixedSystem>(m_window.getWidth(), m_window.getHeight(), fightPos);
	addLogic<PokeballSystem>();
	addLogic<BattleSystem>(m_inputCManager, m_iaICM, fc.fighterPokemon, fc.fighterOpponent, m_pokemon, m_opponent);
	addLogic<SkillRefreshSystem>();
	addLogic<StatisticsSystem>(w, ril, ws, ged);
	addLogic<ska::IARandomMovementSystem>();

	addSubState<StateGUIBattle>();

	//TODO add IA input context ???
	//m_iaICM.addContext(ska::InputContextPtr());
}

ska::CameraSystem& StateFight::getCamera() {
	return *m_cameraSystem;
}


void StateFight::beforeLoad(ska::StatePtr* lastScene) {
	AbstractStateMap::beforeLoad(lastScene);
	if (m_sceneLoaded) {
		return;
	}

	SkillsHolderComponent shc;
	m_skillFactory.loadSkills(m_pokemon, m_pokemonId, shc);
	m_entityManager.addComponent<SkillsHolderComponent>(m_pokemonId, shc);

	SkillsHolderComponent shcOpponent;
	m_skillFactory.loadSkills(m_opponent, m_opponentId, shcOpponent);

	m_entityManager.addComponent<SkillsHolderComponent>(m_opponentId, shcOpponent);
	m_descriptor.load(m_opponent, "Description");

	m_ic = &m_entityManager.getComponent<ska::InputComponent>(m_trainerId);
	m_entityManager.removeComponent<ska::InputComponent>(m_trainerId);


	auto delay = 3000;
	auto dialogTask = std::make_unique<DialogTransition>(delay, m_entityManager, m_eventDispatcher, m_trainerId, "Un " + m_descriptor.getName() + " sauvage apparaît !");
	auto pokeballTask = std::make_unique<PokeballTransition>(delay, m_entityManager, m_pokemonId, m_trainerId, m_pokeball);

	auto finalTask = std::make_unique<ska::Task>([&](ska::Task& t) {
		/* Ajout InputComponent au Pokémon,
		   Ajout d'un IAMovementComponent au dresseur (m_player),
		   Ajout d'un composant de combat au Pokémon
		   Ajout d'une HP Bar */

		m_entityManager.addComponent<BattleComponent>(m_pokemonId, BattleComponent());
		m_entityManager.addComponent<BattleComponent>(m_opponentId, BattleComponent());

		ska::IARandomMovementComponent iarmc;
		iarmc.emitter = m_pokemonId;
		iarmc.delay = 300;
		iarmc.type = ska::RandomMovementType::CIRCLE_AROUND;

		m_entityManager.addComponent<ska::IARandomMovementComponent>(m_trainerId, iarmc);
		m_entityManager.addComponent<ska::InputComponent>(m_pokemonId, *m_ic);
		m_ic = nullptr;

		auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(m_pokemonId);
		auto& pc = m_entityManager.getComponent<ska::PositionComponent>(m_pokeball);
		m_pokeball = 0;
		ska::Rectangle hitbox{pc.x + hc.xOffset, pc.y + hc.yOffset, static_cast<int>(hc.width), static_cast<int>(hc.height)};

		const auto targetBlock = m_worldState.getWorld().placeOnNearestPracticableBlock(hitbox, 1);
		pc.x = targetBlock.x - hc.xOffset;
		pc.y = targetBlock.y - hc.yOffset;
		m_entityManager.addComponent<ska::PositionComponent>(m_pokemonId, pc);

		BattleEvent be(BATTLE_START, *m_cameraSystem, m_pokemonId, m_opponentId, m_entityManager);
		m_eventDispatcher.ska::Observable<BattleEvent>::notifyObservers(be);

		m_sceneLoaded = true;
		m_loadState = 0;
		return false;
	});


	queueTask(dialogTask);
	queueTask(pokeballTask);
	queueTask(finalTask);
}

bool StateFight::beforeUnload() {
	AbstractStateMap::beforeUnload();
	//m_worldState.unload();

	/* Triggers end fight cinematic to the next scene */
	const auto delay = 3000U;

	auto preTask = std::make_unique<ska::Task>([&](ska::Task&) {
		m_ic = &m_entityManager.getComponent<ska::InputComponent>(m_pokemonId);
        m_entityManager.removeComponent<ska::InputComponent>(m_pokemonId);

        BattleEvent be(BATTLE_END, *m_cameraSystem, m_pokemonId, m_opponentId, m_entityManager);
        m_eventDispatcher.ska::Observable<BattleEvent>::notifyObservers(be);

        m_entityManager.removeComponent<BattleComponent>(m_pokemonId);
        m_entityManager.removeComponent<BattleComponent>(m_opponentId);

        if (m_entityManager.hasComponent<ska::DirectionalAnimationComponent>(m_trainerId)) {
            auto& dac = m_entityManager.getComponent<ska::DirectionalAnimationComponent>(m_trainerId);
            dac.type = ska::DirectionalAnimationType::MOVEMENT;
            dac.looked = 0;
        }
        return false;
    });

    std::unique_ptr<ska::CompoundTask> firstTask;
    {
        auto pokeballTask = std::make_unique<PokeballTransition>(delay, m_entityManager, m_pokemonId, m_trainerId, m_pokeball);
        auto dialogTask = std::make_unique<DialogTransition>(delay, m_entityManager, m_eventDispatcher, m_trainerId, "Le " + m_descriptor.getName() + " a été battu.");
        firstTask = std::make_unique<ska::meta::un_unique_type<decltype(firstTask)>::raw>(std::move(pokeballTask), std::move(dialogTask));
    }

	auto finalTask = std::make_unique<ska::Task>([&](ska::Task&) {

		m_loadState = 0;
		m_entityManager.removeComponent<ska::IARandomMovementComponent>(m_trainerId);
		m_entityManager.addComponent<ska::InputComponent>(m_trainerId, *m_ic);
		m_ic = nullptr;
		m_entityManager.removeEntity(m_pokemonId);
		return false;
	});

	if (m_sceneLoaded) {
		m_sceneLoaded = false;
		queueTask(preTask);
		queueTask(firstTask);
		queueTask(finalTask);
	}
	return false;
}

StateFight::~StateFight() {
}
