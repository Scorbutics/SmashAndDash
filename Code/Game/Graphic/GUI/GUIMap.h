#pragma once
#include "AbstractGameGUI.h"
#include "Utils/Observer.h"
#include "../../Gameplay/Data/SettingsChangeEvent.h"
#include "WindowMouseCursor.h"
#include "../../Gameplay/Data/EntityLoadEvent.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"

class Settings;
class WindowTeam;

using DialogEventObserver = ska::Observer<DialogEvent>;

class GUIMap :
	public AbstractGameGUI,
	public ska::Observer<SettingsChangeEvent>,
	public ska::Observer<EntityLoadEvent>,
	public DialogEventObserver {
public:
	GUIMap(ska::Window& w, ska::InputContextManager& playerICM, PokemonGameEventDispatcher& ged);
	GUIMap& operator=(const GUIMap&) = delete;

	void initButtons(const ska::Window& w);
	bool onSettingsChange(SettingsChangeEvent& sce);
	bool onEntityLoad(EntityLoadEvent& ele);
	bool onDialogEvent(DialogEvent& de);
	void bind(Settings& sets);

	~GUIMap();

private:
	WindowMouseCursor* m_attachedToCursor;
	WindowTeam* m_team;
	ska::TimeScrollableWindowIG<>* m_wAction;

};
