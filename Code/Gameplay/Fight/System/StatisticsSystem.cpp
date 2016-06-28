#include "StatisticsSystem.h"
#include "../../Scene/SceneMap.h"

StatisticsSystem::StatisticsSystem(ska::EntityManager& em, ska::SceneHolder& sceneHolder, ska::InputContextManager& icm, WorldScene& ws) : System(em), 
m_sceneHolder(sceneHolder), m_playerICM(icm), m_worldScene(ws) {

}

void StatisticsSystem::refresh() {
	for (ska::EntityId entityId : m_processed) {
		BattleComponent& bc = m_entityManager.getComponent<BattleComponent>(entityId);
		
		/* TODO handle more stats effects */
		if (bc.hp < 0) {
			scheduleDeferredRemove(entityId);
			m_sceneHolder.nextScene(ska::ScenePtr(new SceneMap(m_sceneHolder, m_playerICM, m_worldScene, m_worldScene.getWorld().getFileName(), m_worldScene.getWorld().getChipset().getName())));
		}
	}
}

StatisticsSystem::~StatisticsSystem() {
}
