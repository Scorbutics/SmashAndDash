#pragma once
#include "Base/Task/Task.h"
#include <memory>
#include "../PokemonGameEventDispatcher.h"
#include "../Fight/BattleComponent.h"
#include "Core/ECS/Basics/Input/InputComponent.h"

class CustomEntityManager;
class WorldState;

namespace ska {
	class InputComponent;
}

class StateFightLoading {
public:
	// "landing" : where the pokeball opened (world pixels) : the Pokemon of the player comes out there
	StateFightLoading(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& ws, ska::EntityId pokemonId, ska::EntityId opponentId, ska::EntityId trainerId, const ska::Point<float>& landing, ska::InputComponent** ic);
	// Battle statistics of the Pokemon of the player and of its opponent, given when the battle starts
	void setFighters(BattleComponent pokemon, BattleComponent opponent);
	std::unique_ptr<ska::Task> load();
	std::unique_ptr<ska::Task> unload();
	virtual ~StateFightLoading() = default;

private:
	PokemonGameEventDispatcher& m_eventDispatcher;
	CustomEntityManager& m_entityManager;
	ska::EntityId m_pokemonId;
	ska::EntityId m_opponentId;
	ska::EntityId m_trainerId;
	const ska::Point<float>& m_landing;
	ska::InputComponent** m_ic;
	WorldState& m_worldState;
	BattleComponent m_pokemonFighter;
	ska::InputComponent m_input;
	BattleComponent m_opponentFighter;

};
