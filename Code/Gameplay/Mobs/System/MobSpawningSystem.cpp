#include "MobSpawningSystem.h"
#include "../../../Utils\ChargementImages.h"
#include "../../../Utils\IDs.h"
#include "..\../World\WorldImpl.h"

MobSpawningSystem::MobSpawningSystem(WorldImpl& world, ska::EntityManager& entityManager, const unsigned int delay) : ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, MobSpawnAreaComponent>(entityManager), m_world(world) {
	m_t0 = ska::TimeUtils::getTicks();
	m_duration = delay;
	m_rmin = 8 * TAILLEBLOC;
	m_rmax = m_rmin + 5 * TAILLEBLOC;
	m_spawnNum = 1;
	m_totalSpawnedEntities = 0;
}

void MobSpawningSystem::refresh() {
	if (/*m_totalSpawnedEntities < SPAWN_LIMIT_ALLOWED &&*/ ska::TimeUtils::getTicks() - m_t0 >= m_duration) {
		ska::IniReader* reader = GetRandomMobSettings(&m_world);
		if (reader != NULL) {
			for (ska::EntityId entityId : m_processed) {
				ska::PositionComponent& pc = m_entityManager.getComponent<ska::PositionComponent>(entityId);
				m_totalSpawnedEntities += m_world.spawnMob({ pc.x, pc.y }, m_rmin, m_rmax, m_spawnNum, reader);
			}
		}
		m_t0 = ska::TimeUtils::getTicks();
	}
}

MobSpawningSystem::~MobSpawningSystem() {
}
