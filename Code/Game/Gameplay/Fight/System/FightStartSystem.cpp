#include "FightStartSystem.h"
#include "../../../Utils/IDs.h"
#include "Utils/RectangleUtils.h"
#include "Utils/TimeUtils.h"
#include "Utils/NumberUtils.h"
#include "../../Scene/SceneFight.h"
#include "../../World/WorldScene.h"
#include "../../CustomEntityManager.h"
#include "../../Data/MapEvent.h"

FightStartSystem::FightStartSystem(CustomEntityManager& cem, ska::Window& w, PokemonGameEventDispatcher& ged, WorldScene& ws, ska::InputContextManager& icm, const ska::EntityId player) :
System<std::unordered_set<ska::EntityId>, ska::PositionComponent, FightComponent, ska::GraphicComponent>(cem),
m_cem(cem),
m_worldScene(ws),
m_icm(icm),
m_player(player),
m_window(w),
m_ged(ged) {
	m_t0 = ska::TimeUtils::getTicks();
}

void FightStartSystem::refresh() {
	ska::PositionComponent& pcPlayer = m_entityManager.getComponent<ska::PositionComponent>(m_player);
	ska::GraphicComponent& gc = m_entityManager.getComponent<ska::GraphicComponent>(m_player);
	ska::Point<int> pPlayer = { pcPlayer.x, pcPlayer.y };

	if (gc.sprite.empty()) {
		return;
	}

	pPlayer.x += gc.sprite[0].getWidth() / 2;
	pPlayer.y += gc.sprite[0].getHeight() / 2;


	const unsigned int currentDelay = ska::TimeUtils::getTicks() - m_t0;
	for (ska::EntityId entityId : m_processed) {
		ska::PositionComponent& pc = m_entityManager.getComponent<ska::PositionComponent>(entityId);
		ska::GraphicComponent& gc = m_entityManager.getComponent<ska::GraphicComponent>(entityId);
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
					FightComponent& fc = m_entityManager.getComponent<FightComponent>(entityId);
					/* TODO first available pokemon of the team */
					fc.pokemonScriptId = 25;
					fc.trainer = m_player;
					const int blockSize = m_worldScene.getWorld().getBlockSize();
					fc.fighterPokemon = m_cem.createCharacter(ska::Point<int>(pc.x / blockSize, pc.y / blockSize), fc.pokemonScriptId, blockSize);
					m_entityManager.removeComponent<ska::PositionComponent>(fc.fighterPokemon);
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
