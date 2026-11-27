#include "Game/__internalConfig/LoggerConfig.h"
#include <algorithm>
#include <cmath>
#include "../System/TallGrassSystem.h"
#include "../System/SilhouetteSystem.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Graphic/System/CameraFixedStrategy.h"
#include "../World/WorldState.h"
#include "StateFight.h"
#include "StateGUIBattle.h"
#include "Base/Task/Task.h"
#include "Base/Task/CompoundTask.h"
#include "../Fight/FightComponent.h"
#include "../Data/PokemonDescriptor.h"
#include "Transition/Map/PokeballTransition.h"
#include "../Fight/System/TrainerItemSystem.h"
#include "../Fight/System/TrainerCoachSystem.h"
#include "Transition/Map/DialogTransition.h"
#include "Transition/Map/LeapTransition.h"
#include "../Mobs/WildPokemonComponent.h"
#include "../Fight/System/FightStartSystem.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"

#include "../Fight/System/SkillRefreshSystem.h"
#include "../Fight/System/BattleSystem.h"
#include "../Fight/BattleRules.h"
#include "../Control/MovementModeComponent.h"
#include "../Control/WalkFeelSystem.h"
#include "../Control/FollowerSystem.h"
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
#include "Scripting/ScriptVM.h"

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
	m_loader(m_entityManager, m_eventDispatcher, m_worldState, m_pokemonId, m_opponentId, m_trainerId, m_landing, &m_ic),
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

namespace {
	// A line of the dialogs of the battle, from the script of its steps (Data/Battle/steps.miniska : message), else the
	// fallback
	std::string StepMessage(WorldState& worldState, const std::string& step, const std::string& name, std::string fallback) {
		try {
			return worldState.getScripts().call<std::string>(BattleDirectorSystem::StepsFile, "message",
				{ ska::NodeValue{ std::make_shared<std::string>(step) }, ska::NodeValue{ std::make_shared<std::string>(name) } });
		} catch (std::exception& e) {
			SLOG_STATIC(ska::LogLevel::Warn, StateFight) << "Battle : no message for the step " << step << " : " << e.what();
			return fallback;
		}
	}
}

StateFight::~StateFight() {
	if (m_worldState.battleStage() == &m_stage) {
		m_worldState.setBattleStage(nullptr);
	}
}

void StateFight::beforeLoad(ska::State* lastScene) {
	m_worldState.setBattleStage(&m_stage);
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
	// The fighters in tall grass too
	addLogic(std::make_unique<TallGrassSystem>(m_entityManager, m_worldState));
	// The characters hidden behind the objects of the map : their silhouette
	addLogic(std::make_unique<SilhouetteSystem>(m_entityManager, m_worldState));
	auto& rules = m_worldState.getBattleRules();
	const auto blockSize = static_cast<unsigned int>(m_worldState.getWorld().getBlockSize());

	// The fighters : the Pokemon of the player (keys, or the AI with SAD_BATTLE_AUTOPLAY), the wild Pokemon (AI)
	m_stage.player = m_pokemonId;
	m_stage.trainer = m_trainerId;
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
	// SAD_BATTLE_PASSIVE (environment) : the wild Pokemon do nothing (to test a battle : moving around, the drawing)
	opponentState.controlledByAI = std::getenv("SAD_BATTLE_PASSIVE") == nullptr;
	// The AI of the fighters do not decide at the same time
	opponentState.nextDecision = 150;
	m_entityManager.addComponent<FighterComponent>(m_opponentId, std::move(opponentState));
	createCompanions(rules);
	// The fighters move freely : sorted by their position, not by their block
	for (const auto fighter : m_stage.fighters) {
		if (m_entityManager.hasComponent<ska::GraphicComponent>(fighter)) {
			m_entityManager.getComponent<ska::GraphicComponent>(fighter).sortStep = 0;
		}
	}
	m_stage.canFlee = m_canFlee;
	m_stage.setSoundPlayer([this](BattleSound sound) { m_sounds.play(sound); });
	trainerBlocks(false);

	// The director first : it updates the time of the battle (hit stops) used by the others
	addLogic(std::make_unique<BattleDirectorSystem>(m_entityManager, m_worldState, m_eventDispatcher, m_stage, m_pokemonTeamIndex));
	auto battleSystem = std::make_unique<BattleSystem>(m_entityManager, m_eventDispatcher, rules, m_stage, blockSize);
	auto& battle = *battleSystem;
	addLogic(std::move(battleSystem));
	// The items of the trainer (a potion, a pokeball)
	addLogic(std::make_unique<TrainerItemSystem>(m_entityManager, m_eventDispatcher, m_worldState, m_stage));
	// The trainer follows the fight from behind its Pokemon and comments it (Data/Battle/lines.miniska)
	addLogic(std::make_unique<TrainerCoachSystem>(m_entityManager, m_worldState, m_stage));
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
	// Its walk on the map is over : its drawing is back to normal (WalkFeelSystem)
	if (m_entityManager.hasComponent<ska::GraphicComponent>(m_trainerId)) {
		WalkFeelSystem::reset(m_entityManager.getComponent<ska::GraphicComponent>(m_trainerId));
	}
	// The Pokemon that follows the trainer comes out of its pokeball in the battle : it comes back after it
	FollowerSystem::dismiss(m_entityManager, m_worldState);
	m_ic = &m_input;
	m_entityManager.removeComponent<ska::InputComponent>(m_trainerId);


	// The intro : the wild Pokemon touched jumps back (the room of the battle), it appears (a dialog), the trainer throws
	// its pokeball next to it, the Pokemon of the player comes out of the light
	const auto blockPixels = static_cast<float>(m_worldState.getWorld().getBlockSize());
	const auto& trainerPosition = m_entityManager.getComponent<ska::PositionComponent>(m_trainerId);
	const auto& opponentPosition = m_entityManager.getComponent<ska::PositionComponent>(m_opponentId);
	const auto gap = static_cast<int>(std::lround((std::abs(static_cast<float>(trainerPosition.x - opponentPosition.x)) + std::abs(static_cast<float>(trainerPosition.y - opponentPosition.y))) / blockPixels));
	if (gap < IntroGap) {
		queueTask(std::make_unique<LeapTransition>(m_entityManager, m_worldState, m_opponentId, m_trainerId, IntroGap - gap));
	}
	queueTask(std::make_unique<DialogTransition>(0, m_entityManager, m_eventDispatcher, m_trainerId, StepMessage(m_worldState, "intro", m_descriptor.getName(), "Un " + m_descriptor.getName() + " sauvage apparaît !")));
	queueTask(std::make_unique<PokeballTransition>(m_entityManager, [this]() { return throwPath(); }, m_landing));
	queueTask(m_loader.load());
}

ska::Point<int> StateFight::landingBlock() {
	// The block next to the wild Pokemon, on the side of the trainer (throwPath)
	const auto blockSize = static_cast<int>(m_worldState.getWorld().getBlockSize());
	const auto& trainer = m_entityManager.getComponent<ska::PositionComponent>(m_trainerId);
	const auto& opponent = m_entityManager.getComponent<ska::PositionComponent>(m_opponentId);
	const auto dx = trainer.x - opponent.x;
	const auto dy = trainer.y - opponent.y;
	const auto horizontal = std::abs(dx) > std::abs(dy);
	return { static_cast<int>(opponent.x) / blockSize + (horizontal ? (dx >= 0 ? 1 : -1) : 0),
		static_cast<int>(opponent.y) / blockSize + (horizontal ? 0 : (dy >= 0 ? 1 : -1)) };
}

std::pair<ska::Point<float>, ska::Point<float>> StateFight::throwPath() {
	const auto center = [this](ska::EntityId entity) {
		return ska::PositionComponent::getCenterPosition(m_entityManager.getComponent<ska::PositionComponent>(entity), m_entityManager.getComponent<ska::HitboxComponent>(entity));
	};
	const auto blockSize = static_cast<float>(m_worldState.getWorld().getBlockSize());
	const auto from = center(m_trainerId);
	const auto opponent = center(m_opponentId);
	// The block next to the wild Pokemon, on the side of the trainer (on the axis where they are the farthest apart)
	const auto dx = from.x - opponent.x;
	const auto dy = from.y - opponent.y;
	const auto horizontal = std::abs(dx) > std::abs(dy);
	auto to = horizontal ? ska::Point<float>{ opponent.x + (dx >= 0.F ? blockSize : -blockSize), opponent.y } :
		ska::Point<float>{ opponent.x, opponent.y + (dy >= 0.F ? blockSize : -blockSize) };
	// Too close to the trainer (the wild Pokemon could not jump back) : between them
	if (std::abs(to.x - from.x) + std::abs(to.y - from.y) < blockSize * 0.9F) {
		to = ska::Point<float>{ (from.x + opponent.x) / 2.F, (from.y + opponent.y) / 2.F };
	}
	// Not free (a wall, the water, a character) : where its physics body already is (a free block next to the trainer,
	// FightStartSystem)
	const auto landing = ska::Point<int>{ static_cast<int>(to.x / blockSize), static_cast<int>(to.y / blockSize) };
	const auto own = ska::Point<int>{ m_pokemonPosition.x / static_cast<int>(blockSize), m_pokemonPosition.y / static_cast<int>(blockSize) };
	if (landing != own && !m_worldState.freeBlock(landing.x, landing.y)) {
		to = ska::Point<float>{ (static_cast<float>(own.x) + 0.5F) * blockSize, (static_cast<float>(own.y) + 0.5F) * blockSize };
	}
	// The trainer faces where it throws (0 : down, 1 : right, 2 : up, 3 : left)
	if (m_entityManager.hasComponent<ska::AnimationComponent>(m_trainerId)) {
		m_entityManager.getComponent<ska::AnimationComponent>(m_trainerId).state = horizontal ? (dx >= 0.F ? 3 : 1) : (dy >= 0.F ? 2 : 0);
	}
	return { from, to };
}

void StateFight::park(ska::EntityId entity) {
	if (m_entityManager.hasComponent<ska::cp::HitboxComponent>(entity)) {
		CustomEntityManager::place(m_entityManager, m_worldState.getSpace(), entity, ska::Point<float>{ -100000.F, -100000.F });
	}
}

void StateFight::beforeUnload() {
	// The end of the battle was played in the battle (BattlePhase::Outro) : the player plays again, on the map as it is
	auto finalTask = std::make_unique<ska::Task>([&](ska::Task&) {
		m_loadState = 0;
		m_entityManager.addComponent<ska::InputComponent>(m_trainerId, ska::InputComponent{ *m_ic });
		SLOG(ska::LogLevel::Info) << "Battle over : the player controls the trainer again";
		m_ic = nullptr;
		park(m_pokemonId);
		m_entityManager.removeEntity(m_pokemonId);
		// The other wild Pokemon of the group existed for the battle only
		for (const auto companion : m_companions) {
			park(companion);
			m_entityManager.removeEntity(companion);
		}
		m_companions.clear();
		// The fighters still on the map are drawn as they are : the effects of the battle are over (a flash, a shrink into a
		// pokeball, a fade...), else they would stay as they were at its end (ex : a wild Pokemon of a few pixels)
		for (const auto fighter : m_stage.fighters) {
			if (m_entityManager.hasComponent<ska::GraphicComponent>(fighter)) {
				m_entityManager.getComponent<ska::GraphicComponent>(fighter).effect = ska::RenderEffect{};
			}
		}
		// A wild Pokemon made for the battle (a script : Battle.start), still there (the player fled, or lost) : gone with it
		if (!m_entityManager.hasComponent<WildPokemonComponent>(m_opponentId) && m_entityManager.hasComponent<ska::PositionComponent>(m_opponentId) &&
			m_entityManager.hasComponent<FightComponent>(m_opponentId)) {
			park(m_opponentId);
			m_entityManager.removeEntity(m_opponentId);
		}
		// The wild Pokemon still there (the player fled, or lost) : it stands still, and leaves the player alone a while
		if (m_entityManager.hasComponent<WildPokemonComponent>(m_opponentId)) {
			if (m_entityManager.hasComponent<ska::GraphicComponent>(m_opponentId)) {
				m_entityManager.getComponent<ska::GraphicComponent>(m_opponentId).sortStep = static_cast<int>(m_worldState.getWorld().getBlockSize());
			}
			m_entityManager.getComponent<WildPokemonComponent>(m_opponentId).calm = FightStartSystem::CalmAfterBattle;
			if (m_entityManager.hasComponent<ska::PositionComponent>(m_opponentId)) {
				const auto& position = m_entityManager.getComponent<ska::PositionComponent>(m_opponentId);
				CustomEntityManager::place(m_entityManager, m_worldState.getSpace(), m_opponentId, ska::Point<float>{ static_cast<float>(position.x), static_cast<float>(position.y) });
			}
		}
		trainerBlocks(true);
		return false;
	});

	queueTask(m_loader.unload());
	queueTask(std::move(finalTask));
}

void StateFight::createCompanions(BattleRules& rules) {
	// Next to the first wild Pokemon, one block below, above, two blocks below...
	if (m_companionsData.empty() || !m_entityManager.hasComponent<ska::PositionComponent>(m_opponentId)) {
		return;
	}
	const auto blockSize = static_cast<int>(m_worldState.getWorld().getBlockSize());
	const auto& origin = m_entityManager.getComponent<ska::PositionComponent>(m_opponentId);
	const auto ox = static_cast<int>(origin.x) / blockSize;
	const auto oy = static_cast<int>(origin.y) / blockSize;
	// The free blocks around the first wild Pokemon (not in a wall, the water, on a character : the physics would push them
	// apart, far, the Pokemon of the player out of the arena), the column next to it first, then wider ; not where the
	// Pokemon of the player lands (the side of the trainer)
	const auto landing = landingBlock();
	auto candidates = std::vector<ska::Point<int>>{};
	for (auto k = 0; k < 6; k++) {
		candidates.push_back({ ox + 1, oy + (k / 2 + 1) * (k % 2 == 0 ? 1 : -1) });
	}
	for (auto radius = 1; radius <= 3; radius++) {
		for (auto y = oy - radius; y <= oy + radius; y++) {
			for (auto x = ox - radius; x <= ox + radius; x++) {
				if (std::max(std::abs(x - ox), std::abs(y - oy)) == radius) {
					candidates.push_back({ x, y });
				}
			}
		}
	}
	auto next = candidates.begin();
	auto index = 0;
	for (const auto& [monster, level] : m_companionsData) {
		while (next != candidates.end() && (*next == landing || !m_worldState.freeBlock(next->x, next->y))) {
			++next;
		}
		if (next == candidates.end()) {
			SLOG(ska::LogLevel::Info) << "Battle : no room for the other wild pokemon of the group";
			break;
		}
		index++;
		const auto block = *next++;
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
		state.controlledByAI = std::getenv("SAD_BATTLE_PASSIVE") == nullptr;
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
