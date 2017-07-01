#pragma once
#include "Core/State/StateBase.h"
#include "../CustomEntityManager.h"
#include "../PokemonGameEventDispatcher.h"
#include "../../Graphic/GUI/GUIMap.h"

class Settings;

class StateGUIMap : 
	public ska::StateBase<CustomEntityManager, PokemonGameEventDispatcher> {
public:
    StateGUIMap(StateData& data, ska::StateHolder& sh);
    void bindGUI(Settings& s);
    virtual ~StateGUIMap() = default;

protected:
	virtual void onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) override;
	virtual void onEventUpdate(unsigned int ellapsedTime) override;

private:
    GUIMap m_gui;
};
