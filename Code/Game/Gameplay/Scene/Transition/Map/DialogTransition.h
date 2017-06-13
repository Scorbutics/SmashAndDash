#pragma once
#include "Task/RepeatableTask.h"
#include "ECS/Basics/Input/InputComponent.h"
#include "../../../PokemonGameEventDispatcher.h"

class CustomEntityManager;

namespace ska {
    class CameraFixedSystem;
}

class DialogTransition : public ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>> {
public:
    DialogTransition(unsigned int delay, CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::CameraSystem* cs, const ska::EntityId& pokemonId, const ska::EntityId& opponentId, const ska::EntityId& trainerId, const std::string& pokemonName);
    virtual ~DialogTransition() = default;

private:
    ska::EntityId m_pokeball;
    int m_loadState;

	const ska::EntityId& m_pokemonId;
	const ska::EntityId& m_opponentId;
	const ska::EntityId& m_trainerId;
    std::string m_pokemonName;
    CustomEntityManager& m_entityManager;
    PokemonGameEventDispatcher& m_eventDispatcher;
    ska::CameraSystem* m_cameraSystem;
};
