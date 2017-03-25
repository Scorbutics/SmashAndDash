#pragma once
#include <unordered_set>
#include "ECS/Basics/Physic/PositionComponent.h"
#include "../MobSpawnAreaComponent.h"
#include "ECS/System.h"

class MobSpawner;

class MobSpawningSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, MobSpawnAreaComponent> {
public:
	MobSpawningSystem(MobSpawner& ms, ska::EntityManager& entityManager, const unsigned int delay);
	virtual ~MobSpawningSystem();
	virtual void refresh() override;

private:
	unsigned int m_t0, m_duration, m_rmin, m_rmax, m_spawnNum, m_totalSpawnedEntities;
	MobSpawner& m_mobSpawner;
};


