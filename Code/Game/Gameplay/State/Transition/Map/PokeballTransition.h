#pragma once
#include "Task/Task.h"
#include "ECS/Basics/Input/InputComponent.h"
#include "../../../PokemonGameEventDispatcher.h"

class CustomEntityManager;

namespace ska {
    class CameraFixedSystem;
}

class PokeballTransition : public ska::Task {
public:
    PokeballTransition(unsigned int delay, CustomEntityManager& em, const ska::EntityId& destinationEntity, const ska::EntityId& sourceEntity, ska::EntityId& pokeballHolder);
    virtual ~PokeballTransition() = default;

private:
	bool run(ska::Task& t);

    ska::EntityId& m_pokeball;
    int m_loadState;

	const ska::EntityId& m_trainerId;
	const ska::EntityId& m_pokemonId;
    CustomEntityManager& m_entityManager;
};
