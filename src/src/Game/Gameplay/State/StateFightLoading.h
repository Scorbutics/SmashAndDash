#pragma once
#include "Task/Task.h"
#include <memory>
#include "../PokemonGameEventDispatcher.h"

class CustomEntityManager;
class WorldState;

namespace ska {
	class CameraSystem;
	class InputComponent;
}

class StateFightLoading {
public:
	StateFightLoading(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& ws, ska::EntityId pokemonId, ska::EntityId opponentId, ska::EntityId trainerId, ska::EntityId& pokeball, ska::InputComponent** ic, ska::CameraSystem** cs);
	std::unique_ptr<ska::Task> load();
	std::unique_ptr<ska::Task> unload();
	virtual ~StateFightLoading() = default;

private:
	PokemonGameEventDispatcher& m_eventDispatcher;
	CustomEntityManager& m_entityManager;
	ska::EntityId m_pokemonId;
	ska::EntityId m_opponentId;
	ska::EntityId m_trainerId;
	ska::EntityId& m_pokeball;
	ska::InputComponent** m_ic;
	WorldState& m_worldState;
	ska::CameraSystem** m_cameraSystem;

};
