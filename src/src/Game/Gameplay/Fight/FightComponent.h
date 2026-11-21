#pragma once
#include "Core/ECS/Component.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/Point.h"
#include <utility>
#include <vector>

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
	// Its index in the team, its hit points
	int pokemonTeamIndex = -1;
	int pokemonHp = 0;
	// Where the Pokemon of the player appears (pixels) : the pokeball is thrown there
	ska::Point<int> pokemonPosition;
	ska::EntityId trainer;
	ska::EntityId fighterOpponent;
	ska::EntityId fighterPokemon;
	// Other wild Pokemon fighting with this one (monster, level) : they appear next to it for the battle only
	std::vector<std::pair<int, unsigned int>> companions;
	// The player can flee (leave the arena) : not in a story battle
	bool canFlee = true;
};

SKA_DECLARE_COMPONENT(FightComponent);
