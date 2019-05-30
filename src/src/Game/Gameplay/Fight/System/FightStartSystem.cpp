#include "Game/__internalConfig/LoggerConfig.h"
#include "FightStartSystem.h"
#include "../../../Utils/IDs.h"
#include "Core/Utils/RectangleUtils.h"
#include "Core/Utils/TimeUtils.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "../../State/StateFight.h"
#include "../../World/WorldState.h"
#include "../../CustomEntityManager.h"
#include "../../Data/MapEvent.h"
#include "Core/ECS/EntityLocator.h"
#include "../../../Script/ScriptConstants.h"

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
					/* Start a fight */
					auto& fc = m_componentAccessor.get<FightComponent>(entityId);
					/* TODO first available pokemon of the team */
					fc.pokemonScriptId = 25;
					fc.trainer = *trainer;
					const int blockSize = m_worldScene.getWorld().getBlockSize();
					fc.fighterPokemon = m_cem.createCharacter(ska::Point<int>(pc.x / blockSize, pc.y / blockSize), fc.pokemonScriptId, blockSize);
					m_componentAccessor.remove<ska::PositionComponent>(fc.fighterPokemon);
					fc.fighterOpponent = entityId;

					MapEvent me(MapEventType::Switch_Battle);
					me.fightComponent = &fc;
					me.position = pcPlayer;
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
