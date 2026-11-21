#pragma once
#include "Core/ECS/Component.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/Point.h"

class FightComponent {
public:
	FightComponent() {
		level = 0;
		pokemonScriptId = 0;
		opponentScriptId = 0;
		fighterOpponent = 0;
		trainer = 0;
		fighterPokemon = 0;
		pokemonExperience = 0;
	}

	// Pokemon of the player (pokemonScriptId) : its experience gives its level

	unsigned int level;
	int opponentScriptId;
	int pokemonScriptId;
	int pokemonExperience;
	// Where the Pokemon of the player appears (pixels) : the pokeball is thrown there
	ska::Point<int> pokemonPosition;
	ska::EntityId trainer;
	ska::EntityId fighterOpponent;
	ska::EntityId fighterPokemon;
};

SKA_DECLARE_COMPONENT(FightComponent);
