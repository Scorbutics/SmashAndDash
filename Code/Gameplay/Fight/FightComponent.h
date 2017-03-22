#pragma once
#include "../../ska/ECS/Component.h"
#include "../../ska/ECS/ECSDefines.h"

class FightComponent : public ska::Component {
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
