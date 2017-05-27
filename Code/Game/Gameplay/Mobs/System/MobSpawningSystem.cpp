#include "MobSpawningSystem.h"
#include "../../../Utils/ChargementImages.h"
#include "../../../Utils/IDs.h"
#include "../../World/WorldImpl.h"
#include "Utils/TimeUtils.h"
#include "../MobSpawner.h"

MobSpawningSystem::MobSpawningSystem(ska::EntityManager& entityManager, MobSpawner& ms, const unsigned int delay) : System<std::unordered_set<ska::EntityId>, ska::PositionComponent, MobSpawnAreaComponent>(entityManager), m_mobSpawner(ms) {
	m_t0 = ska::TimeUtils::getTicks();
	m_duration = delay;
	m_rmin = 8 * TAILLEBLOC;
	m_rmax = m_rmin + 5 * TAILLEBLOC;
	m_spawnNum = 1;
	m_totalSpawnedEntities = 0;
}

void MobSpawningSystem::refresh(unsigned int ellapsedTime) {
	if (m_totalSpawnedEntities < SPAWN_LIMIT_ALLOWED && ska::TimeUtils::getTicks() - m_t0 >= m_duration) {
		ska::IniReader* reader = GetRandomMobSettings(m_mobSpawner);
		if (reader != nullptr) {
			for (ska::EntityId entityId : m_processed) {
				ska::PositionComponent& pc = m_entityManager.getComponent<ska::PositionComponent>(entityId);
				m_totalSpawnedEntities += m_mobSpawner.spawnMob({ pc.x, pc.y, 0, 0}, m_rmin, m_rmax, m_spawnNum, reader);
			}
		}
		m_t0 = ska::TimeUtils::getTicks();
	}
}

MobSpawningSystem::~MobSpawningSystem() {
}
