#pragma once
#include "Core/ECS/Component.h"
#include "Core/ECS/ECSDefines.h"

class FightComponent {
public:
	FightComponent() {
		level = 0;
		pokemonScriptId = 0;
		opponentScriptId = 0;
		fighterOpponent = 0;
		trainer = 0;
		fighterPokemon = 0;
	}

	unsigned int level;
	int opponentScriptId;
	int pokemonScriptId;
	ska::EntityId trainer;
	ska::EntityId fighterOpponent;
	ska::EntityId fighterPokemon;
};

SKA_DECLARE_COMPONENT(FightComponent);
