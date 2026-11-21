#pragma once
#include "Base/Patterns/SubObserver.h"
#include "Core/Data/Events/InputKeyEvent.h"
#include "../PokemonGameEventDispatcher.h"

class WorldState;
class Bag;

// Uses an item of the bag on a Pokemon of the team (index) : a healing item gives hit points (battle rules).
// false (the item is kept) when it has no effect : no such item in the bag, not a healing item, the Pokemon is not hurt.
bool UseItem(WorldState& worldState, Bag& bag, int item, int teamIndex);

// Item shortcuts on the map : the keys of "use object 1" to "use object 6" use the item at this place in the bag
// on the first Pokemon of the team it heals. A message tells what happened.
class ItemShortcuts : public ska::SubObserver<ska::InputKeyEvent> {
public:
	ItemShortcuts(PokemonGameEventDispatcher& ged, WorldState& worldState);
	~ItemShortcuts() override = default;

private:
	bool onKeyInputEvent(ska::InputKeyEvent& event);
	void message(const std::string& text);

	PokemonGameEventDispatcher& m_ged;
	WorldState& m_worldState;
};
