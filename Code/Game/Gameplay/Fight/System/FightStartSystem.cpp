#include "FightStartSystem.h"
#include "../../../Utils/IDs.h"
#include "Utils/RectangleUtils.h"
#include "Utils/TimeUtils.h"
#include "Utils/NumberUtils.h"
#include "../../State/StateFight.h"
#include "../../World/WorldState.h"
#include "../../CustomEntityManager.h"
#include "../../Data/MapEvent.h"

FightStartSystem::FightStartSystem(CustomEntityManager& cem, PokemonGameEventDispatcher& ged, WorldState& ws, const ska::EntityId player) :
	System(cem),
	m_cem(cem),
	m_worldScene(ws),
	m_player(player),
	m_ged(ged) {
	m_t0 = ska::TimeUtils::getTicks();
}

void FightStartSystem::refresh(unsigned int ellapsedTime) {
	ska::PositionComponent& pcPlayer = m_componentAccessor.get<ska::PositionComponent>(m_player);
	ska::GraphicComponent& gc = m_componentAccessor.get<ska::GraphicComponent>(m_player);
	ska::Point<int> pPlayer = { pcPlayer.x, pcPlayer.y };

	if (gc.sprite.empty()) {
		return;
	}

	pPlayer.x += gc.sprite[0].getWidth() / 2;
	pPlayer.y += gc.sprite[0].getHeight() / 2;


	const unsigned int currentDelay = ska::TimeUtils::getTicks() - m_t0;
	const auto& processed = getEntities();
	for (ska::EntityId entityId : processed) {
		ska::PositionComponent& pc = m_componentAccessor.get<ska::PositionComponent>(entityId);
		ska::GraphicComponent& gc = m_componentAccessor.get<ska::GraphicComponent>(entityId);
		ska::Point<int> p = { pc.x, pc.y };

		if (gc.sprite.empty()) {
			continue;
		}

		p.x += gc.sprite[0].getWidth() / 2;
		p.y += gc.sprite[0].getHeight() / 2;

		if (m_player != entityId && ska::RectangleUtils::distanceSquared(pPlayer, p) < BATTLE_START_DISTANCE_SQUARED) {
			if (currentDelay >= BATTLE_START_TICKS) {
				if (ska::NumberUtils::random(1, BATTLE_START_CHANCE) == 1) {
					/* Start a fight */
					auto& fc = m_componentAccessor.get<FightComponent>(entityId);
					/* TODO first available pokemon of the team */
					fc.pokemonScriptId = 25;
					fc.trainer = m_player;
					const int blockSize = m_worldScene.getWorld().getBlockSize();
					fc.fighterPokemon = m_cem.createCharacter(ska::Point<int>(pc.x / blockSize, pc.y / blockSize), fc.pokemonScriptId, blockSize);
					m_componentAccessor.remove<ska::PositionComponent>(fc.fighterPokemon);
					fc.fighterOpponent = entityId;
					
					MapEvent me(MapEvent::BATTLE);
					me.fightComponent = &fc;
					me.fightPos = pcPlayer;
					m_ged.ska::Observable<MapEvent>::notifyObservers(me);
				}
				m_t0 = ska::TimeUtils::getTicks();
				break;
			}
		}

	}
}

FightStartSystem::~FightStartSystem() {
}
