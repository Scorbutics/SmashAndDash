#pragma once
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "../WildPokemonComponent.h"

class WorldState;

// The wild Pokemon far from the player and out of the screen leave the map (World/Chunks/DespawnRules.h), the farthest
// ones past a number of them
class DespawnSystem : public ska::System<ska::RequiredComponent<ska::PositionComponent, WildPokemonComponent>, ska::PossibleComponent<>> {
public:
	// Checked this often (ms)
	static constexpr unsigned int Delay = 1000;

	DespawnSystem(ska::EntityManager& entityManager, WorldState& worldState) : System(entityManager), m_entities(entityManager), m_worldState(worldState) {}
	~DespawnSystem() override = default;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	ska::EntityManager& m_entities;
	WorldState& m_worldState;
	unsigned int m_elapsed = 0;
};
