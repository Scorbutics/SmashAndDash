#include "Game/__internalConfig/LoggerConfig.h"
#include "InputHooks.h"
#include "Core/Inputs/InputContextManager.h"
#include "../World/WorldState.h"

InputHooks::InputHooks(PokemonGameEventDispatcher& ged, WorldState& worldState) :
	SubObserver<ska::InputKeyEvent>(std::bind(&InputHooks::onKeyInputEvent, this, std::placeholders::_1), ged),
	m_worldState(worldState) {
}

bool InputHooks::onKeyInputEvent(ska::InputKeyEvent& event) {
	const auto& actions = event.icm.getActions();
	if (actions[ska::DoAction]) {
		m_worldState.emitPlayerHook(Action);
	}
	if (actions[ska::Jump]) {
		m_worldState.emitPlayerHook(Jump);
	}
	return true;
}
