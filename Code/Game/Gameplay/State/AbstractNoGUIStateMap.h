#pragma once
#include "Core/State/StateBase.h"
#include "Core/State/State.h"
#include "../PokemonGameEventDispatcher.h"

class CustomEntityManager;

class AbstractNoGUIStateMap :
	public ska::StateBase<CustomEntityManager, PokemonGameEventDispatcher> {

public:
	AbstractNoGUIStateMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::StateHolder& sh);
	AbstractNoGUIStateMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::State& oldState);
	AbstractNoGUIStateMap& operator=(const AbstractNoGUIStateMap&) = delete;
	virtual ~AbstractNoGUIStateMap();

protected:
	virtual void onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) override;
	virtual void onEventUpdate(unsigned int ellapsedTime) override;

};

