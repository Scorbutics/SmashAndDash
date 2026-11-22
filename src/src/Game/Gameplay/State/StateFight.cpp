#include "Game/__internalConfig/LoggerConfig.h"
#include <algorithm>
#include "Graphic/System/CameraFixedStrategy.h"
#include "../World/WorldState.h"
#include "StateFight.h"
#include "StateGUIBattle.h"
#include "Base/Task/Task.h"
#include "Base/Task/CompoundTask.h"
#include "../Fight/FightComponent.h"
#include "../Data/PokemonDescriptor.h"
#include "Transition/Map/PokeballTransition.h"
#include "Transition/Map/DialogTransition.h"

#include "../Fight/System/SkillRefreshSystem.h"
#include "../Fight/System/BattleSystem.h"
#include "../Fight/BattleRules.h"
#include "../Control/MovementModeComponent.h"
#include "../Fight/System/SkillHitSystem.h"
#include "../Fight/System/BattleDirectorSystem.h"
#include "../Fight/System/FighterSystem.h"
#include "../Fight/System/BattleCameraSystem.h"
#include "../Fight/FighterComponent.h"
#include "../Fight/BattleComponent.h"
#include "../Fight/SkillsHolderComponent.h"
#include "../CustomEntityManager.h"
#include "Physic/Space.h"
#include "Physic/Shape.h"
#include <chipmunk/chipmunk.h>
#include <cstdlib>

namespace {
	BattleComponent Fighter(BattleRules& rules, const ska::IniReader& data, int monster, int level) {
		const auto base = [&data](const std::string& statistic) {
			const auto key = "BaseStats " + statistic;
			return data.exists(key) ? data.get<int>(key) : 10;
		};
		auto fighter = BattleComponent{};
		fighter.monster = monster;
		fighter.level = level;
		fighter.maxHp = rules.maxHp(base("hp"), level);
		fighter.hp = fighter.maxHp;
		fighter.attack = rules.stat(base("attack"), level);
		fighter.defense = rules.stat(base("defense"), level);
		fighter.speAttack = rules.stat(base("spe_attack"), level);
		fighter.speDefense = rules.stat(base("spe_defense"), level);
		fighter.speed = rules.stat(base("speed"), level);
		fighter.type1 = data.exists("Description type1") ? data.get<std::string>("Description type1") : "Normal";
		fighter.type2 = data.exists("Description type2") ? data.get<std::string>("Description type2") : "Aucun";
		SLOG_STATIC(ska::LogLevel::Info, StateFight) << "Battle : pokemon " << monster << " level " << level << ", " << fighter.maxHp << " hit points";
		return fighter;
	}
}

StateFight::StateFight(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& ws, ska::Point<int> fightPos, FightComponent fc) :
	m_eventDispatcher(ged),
	m_entityManager(em),
	m_worldState(ws),
	m_opponentScriptId(fc.opponentScriptId),
	m_pokemonScriptId(fc.pokemonScriptId),
	m_opponentLevel(fc.level < 1 ? 1 : static_cast<int>(fc.level)),
	m_pokemonExperience(fc.pokemonExperience),
	m_pokemonHp(fc.pokemonHp),
	m_pokemonTeamIndex(fc.pokemonTeamIndex),
	m_pokemonPosition(fc.pokemonPosition),
	m_opponent("./Data/Monsters/" + ska::StringUtils::intToStr(fc.opponentScriptId) + ".ini"),
	m_pokemon("./Data/Monsters/" + ska::StringUtils::intToStr(fc.pokemonScriptId) + ".ini"),
	m_pokemonId(fc.fighterPokemon),
	m_trainerId(fc.trainer),
	m_opponentId(fc.fighterOpponent),
	m_sceneLoaded(false),
	m_loadState(0),
	m_ic(nullptr),
	m_skillFactory(ws, fc.level),
	m_loader(m_entityManager, m_eventDispatcher, m_worldState, m_pokemonId, m_opponentId, m_trainerId, m_pokeball, &m_ic),
	m_fightPos(fightPos),
	m_companionsData(fc.companions),
	m_canFlee(fc.canFlee) {
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
	auto& rules = m_worldState.getBattleRules();
	const auto blockSize = static_cast<unsigned int>(m_worldState.getWorld().getBlockSize());

	// The fighters : the Pokemon of the player (keys, or the AI with SAD_BATTLE_AUTOPLAY), the wild Pokemon (AI)
	m_stage.player = m_pokemonId;
	m_stage.fighters = { m_pokemonId, m_opponentId };
	auto pokemonState = FighterComponent{};
	pokemonState.team = 0;
	pokemonState.opponent = m_opponentId;
	pokemonState.player = true;
	pokemonState.controlledByAI = std::getenv("SAD_BATTLE_AUTOPLAY") != nullptr;
	m_entityManager.addComponent<FighterComponent>(m_pokemonId, std::move(pokemonState));
	auto opponentState = FighterComponent{};
	opponentState.team = 1;
	opponentState.opponent = m_pokemonId;
	opponentState.controlledByAI = true;
	// The AI of the fighters do not decide at the same time
	opponentState.nextDecision = 150;
	m_entityManager.addComponent<FighterComponent>(m_opponentId, std::move(opponentState));
	createCompanions(rules);
	m_stage.canFlee = m_canFlee;
	m_stage.setSoundPlayer([this](BattleSound sound) { m_sounds.play(sound); });
	trainerBlocks(false);

	// The director first : it updates the time of the battle (hit stops) used by the others
	addLogic(std::make_unique<BattleDirectorSystem>(m_entityManager, m_worldState, m_eventDispatcher, m_stage, m_pokemonTeamIndex));
	auto battleSystem = std::make_unique<BattleSystem>(m_entityManager, m_eventDispatcher, rules, m_stage, blockSize);
	auto& battle = *battleSystem;
	addLogic(std::move(battleSystem));
	addLogic(std::make_unique<FighterSystem>(m_entityManager, m_eventDispatcher, m_worldState, m_stage));
	addLogic(std::make_unique<BattleCameraSystem>(m_entityManager, m_worldState.getCamera(), m_stage));
	addLogic(std::make_unique<SkillRefreshSystem>(m_entityManager, m_stage));
	addLogic(std::make_unique<SkillHitSystem>(m_entityManager, m_eventDispatcher, rules, m_stage));

	// Statistics of the fighters : from their data and their level, with the battle rules (scripts)
	const auto experienceNeeded = m_pokemon.exists("Experience level_1_exp_needed") ? m_pokemon.get<int>("Experience level_1_exp_needed") : 0;
	const auto pokemonLevel = rules.level(m_pokemonExperience, experienceNeeded);
	auto pokemonFighter = Fighter(rules, m_pokemon, m_pokemonScriptId, pokemonLevel);
	// The Pokemon of the team fights with its hit points left
	if (m_pokemonTeamIndex >= 0) {
		pokemonFighter.hp = std::max(1, std::min(m_pokemonHp, pokemonFighter.maxHp));
	}
	m_loader.setFighters(std::move(pokemonFighter), Fighter(rules, m_opponent, m_opponentScriptId, m_opponentLevel));
	addSubState(std::make_unique<StateGUIBattle>(m_entityManager, m_stage, battle, m_worldState.getCamera()));

	//TODO add IA input context ???
	//m_iaICM.addContext(ska::InputContextPtr());

	SkillsHolderComponent shc;
	SkillFactory{ m_worldState, static_cast<unsigned int>(pokemonLevel) }.loadSkills(m_pokemon, m_pokemonId, shc);
	m_entityManager.addComponent<SkillsHolderComponent>(m_pokemonId, std::move(shc));

	SkillsHolderComponent shcOpponent;
	m_skillFactory.loadSkills(m_opponent, m_opponentId, shcOpponent);

	m_entityManager.addComponent<SkillsHolderComponent>(m_opponentId, std::move(shcOpponent));
	m_descriptor.load(m_opponent, "Description");

	// A copy : the storage of a removed component is reset
	m_input = m_entityManager.getComponent<ska::InputComponent>(m_trainerId);
	// The Pokemon moves with the normal walk of the trainer (not its movement mode : ex "locked" during a cutscene)
	if (m_entityManager.hasComponent<MovementModeComponent>(m_trainerId)) {
		const auto& movementMode = m_entityManager.getComponent<MovementModeComponent>(m_trainerId);
		m_input.movePower = movementMode.walkPower;
	}
	// The jump key dodges in a battle (FighterSystem) : the Pokemon does not jump
	m_input.jumpPower = 0.F;
	m_ic = &m_input;
	m_entityManager.removeComponent<ska::InputComponent>(m_trainerId);


	auto delay = 3000;
	auto dialogTask = std::make_unique<DialogTransition>(delay, m_entityManager, m_eventDispatcher, m_trainerId, "Un " + m_descriptor.getName() + " sauvage apparaît !");
	// The Pokemon of the player has no position yet : the pokeball is thrown where it appears
	auto pokeballTask = std::make_unique<PokeballTransition>(delay, m_entityManager, m_pokemonId, m_trainerId, m_pokeball, m_pokemonPosition);

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
        auto dialogTask = std::make_unique<DialogTransition>(delay, m_entityManager, m_eventDispatcher, m_trainerId, m_worldState.lastBattleWon() ?
			"Le " + m_descriptor.getName() + " a été battu." : (m_worldState.lastBattleFled() ? "Vous prenez la fuite !" : "Votre Pokémon est K.O."));
        firstTask = std::make_unique<ska::CompoundTask>(std::move(pokeballTask), std::move(dialogTask));
    }

	auto finalTask = std::make_unique<ska::Task>([&](ska::Task&) {
		m_loadState = 0;
		m_entityManager.addComponent<ska::InputComponent>(m_trainerId, ska::InputComponent{ *m_ic });
		SLOG(ska::LogLevel::Info) << "Battle over : the player controls the trainer again";
		m_ic = nullptr;
		m_entityManager.removeEntity(m_pokemonId);
		// The other wild Pokemon of the group existed for the battle only
		for (const auto companion : m_companions) {
			CustomEntityManager::place(m_entityManager, m_worldState.getSpace(), companion, ska::Point<float>{ -100000.F, -100000.F });
			m_entityManager.removeEntity(companion);
		}
		m_companions.clear();
		trainerBlocks(true);
		return false;
	});


	queueTask(std::move(preTask));
	queueTask(std::move(firstTask));
	queueTask(std::move(finalTask));
	
}

void StateFight::createCompanions(BattleRules& rules) {
	// Next to the first wild Pokemon, one block below, above, two blocks below...
	if (m_companionsData.empty() || !m_entityManager.hasComponent<ska::PositionComponent>(m_opponentId)) {
		return;
	}
	const auto blockSize = static_cast<int>(m_worldState.getWorld().getBlockSize());
	const auto& origin = m_entityManager.getComponent<ska::PositionComponent>(m_opponentId);
	auto index = 0;
	for (const auto& [monster, level] : m_companionsData) {
		const auto offset = (index / 2 + 1) * (index % 2 == 0 ? 1 : -1);
		index++;
		const auto block = ska::Point<int>{ static_cast<int>(origin.x) / blockSize + 1, static_cast<int>(origin.y) / blockSize + offset };
		const auto companion = CustomEntityManager::createMovingCharacterNG(m_entityManager, m_worldState.getSpace(), block, monster, blockSize);
		const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(monster) + ".ini" };
		const auto companionLevel = static_cast<int>(level < 1 ? 1 : level);
		m_entityManager.addComponent<BattleComponent>(companion, Fighter(rules, data, monster, companionLevel));
		auto skills = SkillsHolderComponent{};
		SkillFactory{ m_worldState, static_cast<unsigned int>(companionLevel) }.loadSkills(data, companion, skills);
		m_entityManager.addComponent<SkillsHolderComponent>(companion, std::move(skills));
		auto state = FighterComponent{};
		state.team = 1;
		state.opponent = m_pokemonId;
		state.controlledByAI = true;
		state.nextDecision = static_cast<unsigned long>(150 + 90 * index);
		m_entityManager.addComponent<FighterComponent>(companion, std::move(state));
		m_companions.push_back(companion);
		m_stage.fighters.push_back(companion);
	}
	SLOG(ska::LogLevel::Info) << "Battle : " << m_companions.size() << " other wild pokemon in the group";
}

void StateFight::trainerBlocks(bool blocks) {
	if (!m_entityManager.hasComponent<ska::cp::HitboxComponent>(m_trainerId)) {
		return;
	}
	const auto& hitbox = m_entityManager.getComponent<ska::cp::HitboxComponent>(m_trainerId);
	cpShapeSetSensor(&m_worldState.getSpace().getShape(hitbox.shapeIndex).shape(), blocks ? cpFalse : cpTrue);
}
