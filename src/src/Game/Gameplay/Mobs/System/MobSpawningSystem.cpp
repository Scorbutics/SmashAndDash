#include "Game/__internalConfig/LoggerConfig.h"
#include "MobSpawningSystem.h"
#include "../../../Utils/ChargementImages.h"
#include "../../../Utils/IDs.h"
#include "../../World/WorldImpl.h"
#include "Core/Utils/TimeUtils.h"
#include "../MobSpawner.h"
#include "../../World/Chunks/DespawnRules.h"
#include <cmath>

MobSpawningSystem::MobSpawningSystem(ska::EntityManager& entityManager, MobSpawner& ms, const unsigned int delay) :
	System(entityManager),
	m_entities(entityManager),
	m_wild(entityManager),
	m_arriving(entityManager),
	m_mobSpawner(ms) {

	m_t0 = ska::TimeUtils::getTicks();
	m_duration = delay;
	m_rmin = 8 * TAILLEBLOC;
	m_rmax = m_rmin + 5 * TAILLEBLOC;
	m_spawnNum = 1;
}

void MobSpawningSystem::refresh(unsigned int) {
	if (ska::TimeUtils::getTicks() - m_t0 >= m_duration && around() < chunks::SpawnCap) {
		ska::IniReader* reader = GetRandomMobSettings(m_mobSpawner);
		if (reader != nullptr) {
			const auto& processed = getEntities();
			for (ska::EntityId entityId : processed) {
				ska::PositionComponent& pc = m_componentAccessor.get<ska::PositionComponent>(entityId);
				m_mobSpawner.spawnMob(ska::Rectangle{static_cast<int>(pc.x), static_cast<int>(pc.y), 0, 0}, m_rmin, m_rmax, m_spawnNum, reader);
			}
		}
		m_t0 = ska::TimeUtils::getTicks();
	}
}

MobSpawningSystem::~MobSpawningSystem() {
}

int MobSpawningSystem::around() {
	auto count = static_cast<int>(m_arriving.getEntities().size());
	const auto radius = chunks::SpawnRadius * TAILLEBLOC;
	for (const auto area : getEntities()) {
		const auto& center = m_componentAccessor.get<ska::PositionComponent>(area);
		for (const auto wild : m_wild.getEntities()) {
			if (!m_entities.hasComponent<ska::PositionComponent>(wild)) {
				continue;
			}
			const auto& position = m_entities.getComponent<ska::PositionComponent>(wild);
			if (std::hypot(static_cast<float>(position.x - center.x), static_cast<float>(position.y - center.y)) <= radius) {
				count++;
			}
		}
	}
	return count;
}
