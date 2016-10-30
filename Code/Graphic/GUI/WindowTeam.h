#pragma once
#include "../../ska/Graphic/GUI/WindowIG.h"
#include "SlotPokemon.h"

class WindowTeam: public ska::WindowIG {
public:
	WindowTeam(ska::MouseObservable& guiObservable, const ska::Point<int>& boxAbsolutePos);
	void addPokemon(unsigned int pokemonId);
	~WindowTeam() = default;

};



