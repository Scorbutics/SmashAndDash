#include "MobSpawningSystem.h"
#include "../../../Utils/ChargementImages.h"
#include "../../../Utils/IDs.h"
#include "../../World/WorldImpl.h"
#include "../../../ska/Utils/TimeUtils.h"
#include "../MobSpawner.h"

MobSpawningSystem::MobSpawningSystem(MobSpawner& ms, ska::EntityManager& entityManager, const unsigned int delay) : System<std::unordered_set<ska::EntityId>, ska::PositionComponent, MobSpawnAreaComponent>(entityManager), m_mobSpawner(ms) {
	m_t0 = ska::TimeUtils::getTicks();
	m_duration = delay;
	m_rmin = 8 * TAILLEBLOC;
	m_rmax = m_rmin + 5 * TAILLEBLOC;
	m_spawnNum = 1;
	m_totalSpawnedEntities = 0;
}

void MobSpawningSystem::refresh() {
	if (m_totalSpawnedEntities < SPAWN_LIMIT_ALLOWED && ska::TimeUtils::getTicks() - m_t0 >= m_duration) {
		ska::IniReader* reader = GetRandomMobSettings(m_mobSpawner);
		if (reader != NULL) {
			for (ska::EntityId entityId : m_processed) {
				ska::PositionComponent& pc = m_entityManager.getComponent<ska::PositionComponent>(entityId);
				m_totalSpawnedEntities += m_mobSpawner.spawnMob({ pc.x, pc.y }, m_rmin, m_rmax, m_spawnNum, reader);
			}
		}
		m_t0 = ska::TimeUtils::getTicks();
	}
}

MobSpawningSystem::~MobSpawningSystem() {
}
