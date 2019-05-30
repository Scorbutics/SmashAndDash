#pragma once
#include <unordered_set>
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "../MobSpawnAreaComponent.h"
#include "Core/ECS/System.h"
#include "AI/System/IADefinedMovementSystem.h"

class MobSpawner;

class MobSpawningSystem : public ska::System< ska::RequiredComponent<ska::PositionComponent, MobSpawnAreaComponent>, ska::PossibleComponent<>> {
public:
	MobSpawningSystem(ska::EntityManager& entityManager, MobSpawner& ms, const unsigned int delay);
	virtual ~MobSpawningSystem();
	virtual void refresh(unsigned int ellapsedTime) override;

private:
	unsigned int m_t0, m_duration, m_rmin, m_rmax, m_spawnNum, m_totalSpawnedEntities;
	MobSpawner& m_mobSpawner;
};


