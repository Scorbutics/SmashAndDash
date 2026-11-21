#include "Game/__internalConfig/LoggerConfig.h"
#include "StatisticsSystem.h"
#include "Game/Gameplay/State/StateMap.h"
#include "Base/IO/Readers/IniReader.h"
#include "Core/ECS/EntityLocator.h"
#include "Game/Script/ScriptConstants.h"

StatisticsSystem::StatisticsSystem(ska::EntityManager& em, WorldState& ws, PokemonGameEventDispatcher& ged, ska::EntityId pokemon, ska::EntityId opponent) :
	System(em),
	m_entities(em),
	m_worldScene(ws),
	m_ged(ged),
	m_pokemon(pokemon),
	m_opponent(opponent) {
}

void StatisticsSystem::refresh(unsigned int) {
	if (m_ended) {
		return;
	}
	const auto& processed = getEntities();
	for (const auto entityId : processed) {
		auto& bc = m_componentAccessor.get<BattleComponent>(entityId);
		if (bc.hp > 0 || (entityId != m_pokemon && entityId != m_opponent)) {
			continue;
		}

		// The battle ends once, back to the map
		m_ended = true;
		const auto won = entityId == m_opponent;
		SLOG(ska::LogLevel::Info) << "Battle " << (won ? "won" : "lost");
		if (won) {
			scheduleDeferredRemove(entityId);
		}
		m_worldScene.battleEnded(won);

		// Back to the map, where the player is
		MapEvent me(MapEventType::Switch_Map);
		me.mapName = m_worldScene.getMapName();
		const auto mapReader = ska::IniReader{ "./Levels/" + me.mapName + "/" + me.mapName + ".ini" };
		me.chipsetName = mapReader.get<std::string>("Chipset file");
		const auto* trainer = m_worldScene.getEntityLocator().getEntityId(SCRIPT_ENTITY_TRAINER);
		const auto& pc = m_entities.getComponent<ska::PositionComponent>(trainer != nullptr && m_entities.hasComponent<ska::PositionComponent>(*trainer) ? *trainer : entityId);
		me.position = pc;
		m_ged.ska::Observable<MapEvent>::notifyObservers(me);
		return;
	}
}

StatisticsSystem::~StatisticsSystem() {
}
