#include "Game/__internalConfig/LoggerConfig.h"
#include "MobSpawningSystem.h"
#include "../../../Utils/ChargementImages.h"
#include "../../../Utils/IDs.h"
#include "../../World/WorldImpl.h"
#include "Core/Utils/TimeUtils.h"
#include "../MobSpawner.h"

MobSpawningSystem::MobSpawningSystem(ska::EntityManager& entityManager, MobSpawner& ms, const unsigned int delay) :
	System(entityManager),
	m_mobSpawner(ms) {

	m_t0 = ska::TimeUtils::getTicks();
	m_duration = delay;
	m_rmin = 8 * TAILLEBLOC;
	m_rmax = m_rmin + 5 * TAILLEBLOC;
	m_spawnNum = 1;
	m_totalSpawnedEntities = 0;
}

void MobSpawningSystem::refresh(unsigned int) {
	if (m_totalSpawnedEntities < SPAWN_LIMIT_ALLOWED && ska::TimeUtils::getTicks() - m_t0 >= m_duration) {
		ska::IniReader* reader = GetRandomMobSettings(m_mobSpawner);
		if (reader != nullptr) {
			const auto& processed = getEntities();
			for (ska::EntityId entityId : processed) {
				ska::PositionComponent& pc = m_componentAccessor.get<ska::PositionComponent>(entityId);
				m_totalSpawnedEntities += m_mobSpawner.spawnMob(ska::Rectangle{static_cast<int>(pc.x), static_cast<int>(pc.y), 0, 0}, m_rmin, m_rmax, m_spawnNum, reader);
			}
		}
		m_t0 = ska::TimeUtils::getTicks();
	}
}

MobSpawningSystem::~MobSpawningSystem() {
}
