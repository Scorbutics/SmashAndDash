#pragma once
#include <optional>
#include "Core/Point.h"
#include "Base/Task/Task.h"
#include "Core/ECS/Basics/Input/InputComponent.h"
#include "../../../PokemonGameEventDispatcher.h"

class CustomEntityManager;

namespace ska {
    class CameraFixedSystem;
}

class PokeballTransition : public ska::Task {
public:
    // The pokeball is thrown from "sourceEntity" to "destinationEntity", or to "destination" when given (pixels)
    PokeballTransition(unsigned int delay, CustomEntityManager& em, const ska::EntityId& destinationEntity, const ska::EntityId& sourceEntity, ska::EntityId& pokeballHolder, std::optional<ska::Point<int>> destination = {});
    virtual ~PokeballTransition() = default;

private:
	bool run(ska::Task& t);

    ska::EntityId& m_pokeball;
    int m_loadState;

	const ska::EntityId& m_trainerId;
	const ska::EntityId& m_pokemonId;
    CustomEntityManager& m_entityManager;
    std::optional<ska::Point<int>> m_destination;
    // When the pokeball was thrown, when it opened (ticks, 0 : not yet)
    unsigned int m_thrownAt = 0;
    unsigned int m_openedAt = 0;
};
