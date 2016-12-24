#pragma once
#include <memory>

class SlotPokemon;
struct SlotPokemonData {
	unsigned int id;
	std::string level;
	std::string name;
	std::string type1;
	std::string type2;
	std::string hp;
	SlotPokemon* parent;

	SlotPokemonData(): parent(nullptr) {

	}

};
using SlotPokemonDataPtr = std::unique_ptr<SlotPokemonData>;