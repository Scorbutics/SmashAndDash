#pragma once
#include <unordered_set>
#include "../../../ska/Physic/PositionComponent.h"
#include "../MobSpawnAreaComponent.h"
#include "../../../ska/ECS/System.h"

class WorldImpl;

class MobSpawningSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, MobSpawnAreaComponent> {
public:
	MobSpawningSystem(WorldImpl& w, ska::EntityManager& entityManager, const unsigned int delay);
	virtual ~MobSpawningSystem();
	virtual void refresh() override;

private:
	unsigned int m_t0, m_duration, m_rmin, m_rmax, m_spawnNum, m_totalSpawnedEntities;
	WorldImpl& m_world;
};

