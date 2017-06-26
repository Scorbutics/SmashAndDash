#include "StatisticsSystem.h"
#include "../../State/StateMap.h"

StatisticsSystem::StatisticsSystem(ska::EntityManager& em, ska::Window& w, ska::InputContextManager& icm, WorldState& ws, PokemonGameEventDispatcher& ged) : System(em),
m_playerICM(icm), m_worldScene(ws), m_window(w), m_ged(ged) {

}

void StatisticsSystem::refresh(unsigned int ellapsedTime) {
	for (const auto entityId : m_processed) {
		auto& bc = m_entityManager.getComponent<BattleComponent>(entityId);

		/* TODO handle more stats effects */
		/* TODO : observer (eg hp < 0 could mean end trainer battle or end pokémon battle) */
		if (bc.hp < 0) {
			scheduleDeferredRemove(entityId);
			MapEvent me(MapEvent::MAP);
			me.mapName = m_worldScene.getFileName();
			me.chipsetName = m_worldScene.getWorld().getChipset().getName();
			m_ged.ska::Observable<MapEvent>::notifyObservers(me);
		}
	}
}

StatisticsSystem::~StatisticsSystem() {
}
