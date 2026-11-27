#pragma once
#include <memory>
#include <string>

class SlotPokemon;
struct SlotPokemonData {
	unsigned int id;
	std::string level;
	std::string name;
	std::string type1;
	std::string type2;
	std::string hp;
	SlotPokemon* parent;
	// Its place in the team (Team : the files of the save)
	int index = -1;

	SlotPokemonData(): id(0), parent(nullptr) {

	}

};
using SlotPokemonDataPtr = std::unique_ptr<SlotPokemonData>;
