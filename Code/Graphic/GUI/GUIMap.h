#pragma once
#include "AbstractGameGUI.h"
#include "../../ska/Utils/Observer.h"
#include "../../Gameplay/Data/SettingsChangeEvent.h"
#include "WindowMouseCursor.h"
#include "../../Gameplay/Data/EntityLoadEvent.h"

class Settings;
class WindowTeam;

class GUIMap : 
	public AbstractGameGUI, 
	public ska::Observer<SettingsChangeEvent>,
	public ska::Observer<EntityLoadEvent> {
public:
	GUIMap(ska::Window& w, ska::InputContextManager& playerICM, PokemonGameEventDispatcher& ged);
	GUIMap& operator=(const GUIMap&) = delete;

	bool onSettingsChange(SettingsChangeEvent& sce);
	bool onEntityLoad(EntityLoadEvent& ele);
	void bind(Settings& sets);

	~GUIMap();
private:
	WindowMouseCursor* m_attachedToCursor;
	WindowTeam* m_team;

};