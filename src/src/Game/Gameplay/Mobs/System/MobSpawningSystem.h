#pragma once
#include <unordered_set>
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "../MobSpawnAreaComponent.h"
#include "../WildPokemonComponent.h"
#include "Core/ECS/System.h"
#include "AI/System/IADefinedMovementSystem.h"

class MobSpawner;

// The wild Pokemon on the map : its entities are kept by the ECS
class WildPokemons : public ska::System<ska::RequiredComponent<WildPokemonComponent>, ska::PossibleComponent<>> {
public:
	explicit WildPokemons(ska::EntityManager& entityManager) : System(entityManager) {}
	~WildPokemons() override = default;
	using System::getEntities;
protected:
	void refresh(unsigned int) override {}
};

// Wild Pokemon appear around the spawn areas from time to time, up to a number of them on the map (the ones still there :
// a map left and found again after a battle keeps its wild Pokemon)
class MobSpawningSystem : public ska::System< ska::RequiredComponent<ska::PositionComponent, MobSpawnAreaComponent>, ska::PossibleComponent<>> {
public:
	MobSpawningSystem(ska::EntityManager& entityManager, MobSpawner& ms, const unsigned int delay);
	virtual ~MobSpawningSystem();
	virtual void refresh(unsigned int ellapsedTime) override;

private:
	unsigned int m_t0, m_duration, m_rmin, m_rmax, m_spawnNum;
	WildPokemons m_wild;
	MobSpawner& m_mobSpawner;
};


