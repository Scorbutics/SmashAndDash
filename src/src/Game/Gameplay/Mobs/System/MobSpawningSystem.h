#pragma once
#include <unordered_set>
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "../MobSpawnAreaComponent.h"
#include "../WildPokemonComponent.h"
#include "../ArrivalComponent.h"
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

// The wild Pokemon arriving (flying in, out of a hideout) or leaving : drawings, not wild Pokemon yet
class ArrivingPokemons : public ska::System<ska::RequiredComponent<ArrivalComponent>, ska::PossibleComponent<>> {
public:
	explicit ArrivingPokemons(ska::EntityManager& entityManager) : System(entityManager) {}
	~ArrivingPokemons() override = default;
	using System::getEntities;
protected:
	void refresh(unsigned int) override {}
};

// Wild Pokemon appear around the spawn areas from time to time, up to a number of them around each (the ones still there,
// and the ones arriving : a map left and found again after a battle keeps its wild Pokemon)
class MobSpawningSystem : public ska::System< ska::RequiredComponent<ska::PositionComponent, MobSpawnAreaComponent>, ska::PossibleComponent<>> {
public:
	MobSpawningSystem(ska::EntityManager& entityManager, MobSpawner& ms, const unsigned int delay);
	virtual ~MobSpawningSystem();
	virtual void refresh(unsigned int ellapsedTime) override;

private:
	// The wild Pokemon around the spawn areas, and the ones arriving
	int around();

	unsigned int m_t0, m_duration, m_rmin, m_rmax, m_spawnNum;
	ska::EntityManager& m_entities;
	WildPokemons m_wild;
	ArrivingPokemons m_arriving;
	MobSpawner& m_mobSpawner;
};


