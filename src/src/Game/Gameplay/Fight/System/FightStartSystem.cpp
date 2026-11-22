#include "Game/__internalConfig/LoggerConfig.h"
#include "FightStartSystem.h"
#include <cmath>
#include "../../../Utils/IDs.h"
#include "Core/Utils/RectangleUtils.h"
#include "Core/Utils/TimeUtils.h"
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

void FightStartSystem::refresh(unsigned int) {
	auto* trainer = m_entityLocator.getEntityId(SCRIPT_ENTITY_TRAINER);
	if (trainer == nullptr) {
		return;
	}
	ska::PositionComponent& pcPlayer = m_componentAccessor.get<ska::PositionComponent>(*trainer);
	ska::GraphicComponent& gc = m_componentAccessor.get<ska::GraphicComponent>(*trainer);
	ska::Point<float> pPlayer(pcPlayer.x, pcPlayer.y);

	if (gc.animatedSprites.empty()) {
		return;
	}

	pPlayer.x += gc.animatedSprites[0].getWidth() / 2;
	pPlayer.y += gc.animatedSprites[0].getHeight() / 2;


	const unsigned int currentDelay = ska::TimeUtils::getTicks() - m_t0;
	const auto& processed = getEntities();
	for (ska::EntityId entityId : processed) {
		ska::PositionComponent& pc = m_componentAccessor.get<ska::PositionComponent>(entityId);
		ska::GraphicComponent& gc = m_componentAccessor.get<ska::GraphicComponent>(entityId);
		ska::Point<float> p(pc.x, pc.y);

		if (gc.animatedSprites.empty()) {
			continue;
		}

		p.x += gc.animatedSprites[0].getWidth() / 2;
		p.y += gc.animatedSprites[0].getHeight() / 2;

		if (*trainer != entityId && ska::RectangleUtils::distanceSquared(pPlayer, p) < BATTLE_START_DISTANCE_SQUARED) {
			if (currentDelay >= BATTLE_START_TICKS) {
				if (ska::NumberUtils::random(1, BATTLE_START_CHANCE) == 1) {
					// Sometimes the wild Pokemon is not alone : another one of its kind joins the battle
					auto& fc = m_componentAccessor.get<FightComponent>(entityId);
					fc.companions.clear();
					if (ska::NumberUtils::random(1, GroupChance) == 1) {
						const auto level = std::max(1, static_cast<int>(fc.level) + ska::NumberUtils::random(-1, 1));
						fc.companions.emplace_back(fc.opponentScriptId, static_cast<unsigned int>(level));
					}
					StartBattle(m_cem, m_ged, m_worldScene, *trainer, entityId);
				}
				m_t0 = ska::TimeUtils::getTicks();
				break;
			}
		}

	}
}

FightStartSystem::~FightStartSystem() {
}
