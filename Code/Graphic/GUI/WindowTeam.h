#pragma once
#include <list>

#include "SlotPokemon.h"
#include "../../ska/Graphic/GUI/Windows/MoveableWindow.h"

class WindowMouseCursor;
class WindowTeam : public ska::MoveableWindow<ska::ValueChangedEventListener<SlotPokemonDataPtr*>> {
public:
	WindowTeam(Widget& parent, WindowMouseCursor* mouseCursor, const ska::Point<int>& boxAbsolutePos);
	SlotPokemon* insertPokemon(SlotPokemon* before, SlotPokemonDataPtr spd);
	void organizeSlots();
	~WindowTeam() = default;

private:
	void unloadPokemon(SlotPokemon* slot);

	WindowMouseCursor* m_mouseCursor;
	std::vector<SlotPokemon*> m_slots;
	std::list<SlotPokemon*> m_visibleSlots;
};



