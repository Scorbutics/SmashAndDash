#pragma once
#include "Base/Patterns/SubObserver.h"
#include "Core/Data/Events/InputKeyEvent.h"
#include "../PokemonGameEventDispatcher.h"

class WorldState;

// Engine events for the scripts (hooks, with the player) when a key is pressed on the map :
// "input.action" (action key), "input.jump" (jump key). Ex : Engine.on("input.action", function(player: int) do ... end)
class InputHooks : public ska::SubObserver<ska::InputKeyEvent> {
public:
	static constexpr auto Action = "input.action";
	static constexpr auto Jump = "input.jump";

	InputHooks(PokemonGameEventDispatcher& ged, WorldState& worldState);
	~InputHooks() override = default;

private:
	bool onKeyInputEvent(ska::InputKeyEvent& event);
	WorldState& m_worldState;
};
