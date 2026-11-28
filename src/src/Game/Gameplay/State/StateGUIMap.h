#pragma once
#include "Core/State/StateBase.h"
#include "../CustomEntityManager.h"
#include "../PokemonGameEventDispatcher.h"
#include "../../Graphic/GUI/DebugPanel.h"
#include "../../Graphic/GUI/DialogBox.h"
#include "../../Graphic/GUI/TrainerMenu.h"

class Settings;
class WorldState;

// The interface over the map and the battles, drawn with Ui::Canvas : the dialogs (DialogBox), the menu of the trainer
// (TrainerMenu), the panel of the developers (DebugPanel)
class StateGUIMap :
	public ska::StateBase {
public:
	StateGUIMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& worldState, Settings& settings);
    virtual ~StateGUIMap() = default;

protected:
	virtual void onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) override;
	virtual void onEventUpdate(unsigned int ellapsedTime) override;

private:
	DialogBox m_dialogs;
	TrainerMenu m_menu;
	DebugPanel m_debug;
};
