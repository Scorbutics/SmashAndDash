#include "FightStartSystem.h"
#include "../../../Utils/IDs.h"
#include "../../../ska/Utils/RectangleUtils.h"
#include "../../../ska/Utils/TimeUtils.h"
#include "../../../ska/Utils/NumberUtils.h"
#include "../../Scene/SceneFight.h"
#include "../../World/WorldScene.h"
#include "../../CustomEntityManager.h"

FightStartSystem::FightStartSystem(ska::SceneHolder& sceneHolder, WorldScene& ws, ska::InputContextManager& icm, const ska::EntityId player) :
ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, FightComponent, ska::GraphicComponent>(ws.getEntityManager()),
m_worldScene(ws),
m_icm(icm),
m_player(player),
m_sceneHolder(sceneHolder){
	m_t0 = ska::TimeUtils::getTicks();
}

void FightStartSystem::refresh() {
	ska::PositionComponent& pcPlayer = m_entityManager.getComponent<ska::PositionComponent>(m_player);
	ska::GraphicComponent& gc = m_entityManager.getComponent<ska::GraphicComponent>(m_player);
	ska::Point<int> pPlayer = { pcPlayer.x, pcPlayer.y };
	pPlayer.x += gc.sprite.getWidth() / 2;
	pPlayer.y += gc.sprite.getHeight() / 2;
	

	const unsigned int currentDelay = ska::TimeUtils::getTicks() - m_t0;
	for (ska::EntityId entityId : m_processed) {
		ska::PositionComponent& pc = m_entityManager.getComponent<ska::PositionComponent>(entityId);
		ska::GraphicComponent& gc = m_entityManager.getComponent<ska::GraphicComponent>(entityId);
		ska::Point<int> p = { pc.x, pc.y };
		p.x += gc.sprite.getWidth() / 2;
		p.y += gc.sprite.getHeight() / 2;

		if (m_player != entityId && ska::RectangleUtils::distanceSquared(pPlayer, p) < BATTLE_START_DISTANCE_SQUARED) {
			if (currentDelay >= BATTLE_START_TICKS) {
				if (ska::NumberUtils::random(1, BATTLE_START_CHANCE) == 1) {
					/* Start a fight */
					m_sceneHolder.nextScene(ska::ScenePtr(new SceneFight(m_sceneHolder, m_worldScene, m_icm, pc)));
				}
				m_t0 = ska::TimeUtils::getTicks();
				break;
			}
		}

	}
}

FightStartSystem::~FightStartSystem() {
}
