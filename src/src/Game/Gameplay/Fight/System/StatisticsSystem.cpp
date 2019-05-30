#include "Game/__internalConfig/LoggerConfig.h"
#include "StatisticsSystem.h"
#include "Game/Gameplay/State/StateMap.h"

StatisticsSystem::StatisticsSystem(ska::EntityManager& em, WorldState& ws, PokemonGameEventDispatcher& ged) : System(em),
m_worldScene(ws), m_ged(ged) {

}

void StatisticsSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	for (const auto entityId : processed) {
		auto& bc = m_componentAccessor.get<BattleComponent>(entityId);

		/* TODO handle more stats effects */
		/* TODO : observer (eg hp < 0 could mean end trainer battle or end pokémon battle) */
		if (bc.hp < 0) {
			scheduleDeferredRemove(entityId);

			const auto& pc = m_componentAccessor.get<ska::PositionComponent>(entityId);
			MapEvent me(MapEventType::Switch_Map);
			me.mapName = m_worldScene.getFileName();
			me.position = pc;
			me.chipsetName = m_worldScene.getTilesetName();
			m_ged.ska::Observable<MapEvent>::notifyObservers(me);
		}
	}
}

StatisticsSystem::~StatisticsSystem() {
}
