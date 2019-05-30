#pragma once
#include "Core/State/StateBase.h"
#include "../CustomEntityManager.h"
#include "../PokemonGameEventDispatcher.h"
#include "../../Graphic/GUI/GUIBattle.h"

class Settings;

class StateGUIBattle : public ska::StateBase {
public:
	StateGUIBattle(CustomEntityManager& em, PokemonGameEventDispatcher& ged);
    //void bindGUI(Settings& s);
    virtual ~StateGUIBattle() = default;

protected:
	
	virtual void onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) override;
	virtual void beforeUnload() override;
	virtual void onEventUpdate(unsigned int ellapsedTime) override;

private:
	/* TODO GUI Battle specific part with Bars and also skills displayed */
	GUIBattle m_gui;
};
