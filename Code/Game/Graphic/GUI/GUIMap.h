#pragma once
#include "Utils/Observer.h"
#include "../../Gameplay/Data/SettingsChangeEvent.h"
#include "WindowMouseCursor.h"
#include "../../Gameplay/Data/EntityLoadEvent.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"
#include "GUIBattle.h"
#include "GUI/GUI.h"

class Settings;
class WindowTeam;
class SkillsBar;

using DialogEventObserver = ska::Observer<DialogEvent>;

class GUIMap :
	public ska::GUI,
	public ska::Observer<SettingsChangeEvent>,
	public ska::Observer<EntityLoadEvent>,
	public DialogEventObserver,
	public BattleStartObserver {

public:
	GUIMap(PokemonGameEventDispatcher& ged);
	GUIMap& operator=(const GUIMap&) = delete;

	void initButtons();
	bool onSettingsChange(SettingsChangeEvent& sce);
	bool onEntityLoad(EntityLoadEvent& ele);
	bool onBattleStart(BattleEvent& be);
	bool onDialogEvent(DialogEvent& de);
	virtual bool onScreenResized(unsigned int width, unsigned int height) override;
	void bind(Settings& sets);

	~GUIMap();

private:
	PokemonGameEventDispatcher& m_ged;
	WindowMouseCursor* m_attachedToCursor;
	WindowTeam* m_team;
	ska::TimeScrollableWindowIG<>* m_wAction;
	SkillsBar* m_skillBar;

};
