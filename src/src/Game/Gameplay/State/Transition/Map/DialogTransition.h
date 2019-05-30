#pragma once
#include "Base/Task/Task.h"
#include "Core/ECS/Basics/Input/InputComponent.h"
#include "../../../PokemonGameEventDispatcher.h"

class CustomEntityManager;

namespace ska {
    class CameraFixedSystem;
}

class DialogTransition : 
	public ska::Task {
public:
    DialogTransition(unsigned int delay, CustomEntityManager& em, PokemonGameEventDispatcher& ged, const ska::EntityId& trainerId, const std::string& message);
    virtual ~DialogTransition() = default;

private:
    int m_loadState;

	const ska::EntityId& m_trainerId;
    std::string m_message;
    CustomEntityManager& m_entityManager;
    PokemonGameEventDispatcher& m_eventDispatcher;
};
