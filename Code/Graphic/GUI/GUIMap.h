#pragma once
#include "AbstractGameGUI.h"
#include "../../ska/Utils/Observer.h"
#include "../../Gameplay/Data/SettingsChangeEvent.h"
#include "WindowMouseCursor.h"

class Settings;
class WindowTeam;

class GUIMap : 
	public AbstractGameGUI, 
	public ska::Observer<SettingsChangeEvent> {
public:
	GUIMap(ska::Window& w, ska::InputContextManager& playerICM, PokemonGameEventDispatcher& ged);
	
	bool onSettingsChange(SettingsChangeEvent& sce);

	void bind(Settings& sets);

	~GUIMap();
private:
	WindowMouseCursor* m_attachedToCursor;
	WindowTeam* m_team;

};