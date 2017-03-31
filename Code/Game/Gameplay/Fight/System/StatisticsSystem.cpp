#include "StatisticsSystem.h"
#include "../../Scene/SceneMap.h"

StatisticsSystem::StatisticsSystem(ska::Window& w, ska::EntityManager& em, ska::SceneHolder& sceneHolder, ska::InputContextManager& icm, WorldScene& ws, PokemonGameEventDispatcher& ged) : System(em),
m_sceneHolder(sceneHolder), m_playerICM(icm), m_worldScene(ws), m_window(w), m_ged(ged) {

}

void StatisticsSystem::refresh() {
	for (ska::EntityId entityId : m_processed) {
		BattleComponent& bc = m_entityManager.getComponent<BattleComponent>(entityId);

		/* TODO handle more stats effects */
		/* TODO : observer (eg hp < 0 could mean end trainer battle or end pokémon battle) */
		if (bc.hp < 0) {
			scheduleDeferredRemove(entityId);
			
			/*auto scene = ska::ScenePtr(new SceneMap(m_window, m_sceneHolder, m_ged, m_playerICM, m_worldScene, m_worldScene.getFileName(), m_worldScene.getWorld().getChipset().getName(), true));
			m_sceneHolder.nextScene(scene);*/
		}
	}
}

StatisticsSystem::~StatisticsSystem() {
}
