#pragma once
#include "../../ska/Graphic/GUI/MoveableWindow.h"
#include "SlotPokemon.h"

class WindowTeam : public ska::MoveableWindow<ska::ValueChangedEventListener<SlotPokemonData>> {
public:
	WindowTeam(ska::Widget& parent, const ska::Point<int>& boxAbsolutePos);
	void loadPokemon(unsigned int slot, unsigned int pokemonId);
	void unloadPokemon(unsigned int slot);
	~WindowTeam() = default;
private:
	unsigned int m_pokemonCount;
	std::vector<SlotPokemon*> m_slots;
};



