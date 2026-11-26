#include "Game/__internalConfig/LoggerConfig.h"
#include "FightStartSystem.h"
#include <cmath>
#include "../../../Utils/IDs.h"
#include "Core/Utils/RectangleUtils.h"
#include "Core/Utils/TimeUtils.h"
#include "Core/ECS/Basics/Input/InputComponent.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "../../State/StateFight.h"
#include "../../World/WorldState.h"
#include "../../CustomEntityManager.h"
#include "../../Data/MapEvent.h"
#include "Core/ECS/EntityLocator.h"
#include "Base/IO/Readers/IniReader.h"
#include "../../Data/SavegameManager.h"
#include "../../Data/Team.h"
#include "../../../Script/ScriptConstants.h"
#include "Game/Gameplay/Fight/BattleRules.h"

bool StartBattle(CustomEntityManager& cem, PokemonGameEventDispatcher& ged, WorldState& ws, ska::EntityId trainer, ska::EntityId opponent) {
	auto& fc = cem.getComponent<FightComponent>(opponent);
	// The Pokemon of the player : the first of its team able to fight
	const auto team = Team{ ws.getSaveGame().getPathName() };
	const auto teamIndex = team.firstAble();
	if (!teamIndex.has_value()) {
		SLOG_STATIC(ska::LogLevel::Info, FightStartSystem) << "Battle : no pokemon of the team can fight";
		return false;
	}
	const auto pokemon = *team.member(*teamIndex);
	fc.pokemonScriptId = pokemon.id;
	fc.pokemonExperience = pokemon.experience;
	fc.pokemonHp = pokemon.hp;
	fc.pokemonTeamIndex = *teamIndex;
	fc.trainer = trainer;
	const auto& pc = cem.getComponent<ska::PositionComponent>(opponent);
	const auto& pcPlayer = cem.getComponent<ska::PositionComponent>(trainer);
	const int blockSize = ws.getWorld().getBlockSize();
	// It appears next to the player, towards the wild Pokemon (it has no position until the pokeball opens, but its physics
	// body is already there)
	const auto dx = pc.x - pcPlayer.x;
	const auto dy = pc.y - pcPlayer.y;
	const auto horizontal = std::abs(dx) >= std::abs(dy);
	fc.pokemonPosition = ska::Point<int>(static_cast<int>(pcPlayer.x) + (horizontal ? (dx >= 0 ? blockSize : -blockSize) : 0),
		static_cast<int>(pcPlayer.y) + (horizontal ? 0 : (dy >= 0 ? blockSize : -blockSize)));
	fc.fighterPokemon = CustomEntityManager::createMovingCharacterNG(cem, ws.getSpace(), ska::Point<int>(fc.pokemonPosition.x / blockSize, fc.pokemonPosition.y / blockSize), fc.pokemonScriptId, blockSize);
	cem.removeComponent<ska::PositionComponent>(fc.fighterPokemon);
	fc.fighterOpponent = opponent;
	SLOG_STATIC(ska::LogLevel::Info, FightStartSystem) << "Battle : pokemon " << fc.pokemonScriptId << " against pokemon " << fc.opponentScriptId << " level " << fc.level;

	MapEvent me(MapEventType::Switch_Battle);
	me.fightComponent = &fc;
	me.position = pcPlayer;
	ged.ska::Observable<MapEvent>::notifyObservers(me);
	return true;
}

FightStartSystem::FightStartSystem(CustomEntityManager& cem, PokemonGameEventDispatcher& ged, WorldState& ws, const ska::EntityLocator& entityLocator) :
	System(cem),
	m_cem(cem),
	m_worldScene(ws),
	m_entityLocator(entityLocator),
	m_ged(ged) {
	m_t0 = ska::TimeUtils::getTicks();
}

void FightStartSystem::refresh(unsigned int ellapsedTime) {
	auto* trainer = m_entityLocator.getEntityId(SCRIPT_ENTITY_TRAINER);
	if (trainer == nullptr || !m_cem.hasComponent<ska::PositionComponent>(*trainer)) {
		return;
	}
	const auto* trainerGrid = m_cem.hasComponent<GridMovementComponent>(*trainer) ? &m_cem.getComponent<GridMovementComponent>(*trainer) : nullptr;
	// Not right after the map appeared, nor while the player does not play (a cutscene, a map change)
	const auto ready = ska::TimeUtils::getTicks() - m_t0 >= BATTLE_START_TICKS && m_cem.hasComponent<ska::InputComponent>(*trainer) &&
		!m_worldScene.getFade().running();

	for (const auto entityId : getEntities()) {
		auto& wild = m_componentAccessor.get<WildPokemonComponent>(entityId);
		wild.calm = wild.calm > ellapsedTime ? wild.calm - ellapsedTime : 0;
		auto* grid = m_componentPossibleAccessor.get<GridMovementComponent>(entityId);
		if (grid != nullptr) {
			ReactToPlayer(*grid, wild.calm > 0 ? WildPokemonComponent::Attitude::Passive : wild.attitude, *trainer);
		}
		if (!ready || wild.calm > 0) {
			continue;
		}
		const auto touched = (trainerGrid != nullptr && trainerGrid->touched == std::optional<ska::EntityId>{ entityId }) ||
			(grid != nullptr && grid->touched == std::optional<ska::EntityId>{ *trainer });
		if (!touched) {
			continue;
		}
		SLOG(ska::LogLevel::Info) << "Battle : the trainer and a " << AttitudeName(wild.attitude) << " wild pokemon touch";
		// The rules of the encounters (Data/Battle/encounters.miniska) : sometimes the wild Pokemon is not alone, others of
		// its kind join the battle
		auto& rules = m_worldScene.getBattleRules();
		auto& fc = m_componentAccessor.get<FightComponent>(entityId);
		fc.companions.clear();
		if (ska::NumberUtils::random(1, rules.groupOdds()) == 1) {
			const auto spread = rules.levelSpread();
			for (auto i = 0, size = rules.groupSize(); i < size; i++) {
				const auto level = rules.companionLevel(static_cast<int>(fc.level), ska::NumberUtils::random(-spread, spread));
				fc.companions.emplace_back(fc.opponentScriptId, static_cast<unsigned int>(level));
			}
		}
		if (!StartBattle(m_cem, m_ged, m_worldScene, *trainer, entityId)) {
			// No Pokemon of the team can fight : it leaves the player alone for a while
			wild.calm = CalmAfterBattle;
		}
		break;
	}
}

FightStartSystem::~FightStartSystem() {
}
