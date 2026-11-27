#pragma once
#include <functional>
#include <list>

#include "SlotPokemon.h"
#include "GUI/Windows/MoveableWindow.h"

class WindowMouseCursor;
class WindowTeam : public ska::MoveableWindow<ska::ValueChangedEventListener<SlotPokemonDataPtr*>> {
public:
	WindowTeam(ska::WidgetPanel& parent, WindowMouseCursor* mouseCursor, const ska::Point<int>& boxAbsolutePos);
	// A Pokemon shown at "position" in the team (-1 : at its end)
	SlotPokemon* insertPokemon(SlotPokemonDataPtr spd, int position = -1);
	void organizeSlots();
	// The order of the team : a Pokemon taken (a click) is put down on another one (a click) : it takes its place, the
	// others follow. "moved" changes the team (from, to)
	void onMove(std::function<void(int, int)> moved) { m_moved = std::move(moved); }
	~WindowTeam() = default;

private:
	void unloadPokemon(SlotPokemon* slot);

	WindowMouseCursor* m_mouseCursor;
	std::vector<SlotPokemon*> m_slots;
	std::list<SlotPokemon*> m_visibleSlots;
	std::function<void(int, int)> m_moved;
};



