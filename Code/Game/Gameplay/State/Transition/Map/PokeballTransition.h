#pragma once
#include "Task/RepeatableTask.h"
#include "ECS/Basics/Input/InputComponent.h"
#include "../../../PokemonGameEventDispatcher.h"

class CustomEntityManager;

namespace ska {
    class CameraFixedSystem;
}

class PokeballTransition : public ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>> {
public:
    PokeballTransition(unsigned int delay, CustomEntityManager& em, const ska::EntityId& destinationEntity, const ska::EntityId& sourceEntity);
    virtual ~PokeballTransition() = default;

private:
    ska::EntityId m_pokeball;
    int m_loadState;

	const ska::EntityId& m_trainerId;
	const ska::EntityId& m_pokemonId;
    CustomEntityManager& m_entityManager;
};
