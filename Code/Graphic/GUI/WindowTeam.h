#pragma once
#include "../../ska/Graphic/GUI/MoveableWindow.h"
#include "SlotPokemon.h"

class WindowTeam : public ska::MoveableWindow<> {
public:
	WindowTeam(ska::Widget& parent, const ska::Point<int>& boxAbsolutePos);
	void addPokemon(unsigned int pokemonId);
	~WindowTeam() = default;
private:
	unsigned int m_pokemonCount;
};



