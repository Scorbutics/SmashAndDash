#pragma once
#include "Base/Patterns/Observer.h"
#include "../../Gameplay/Data/SettingsChangeEvent.h"
#include "WindowMouseCursor.h"
#include "../../Gameplay/Data/EntityLoadEvent.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"
#include "GUIBattle.h"
#include "GUI/GUI.h"
#include "GUI/Windows/DebugWindow.h"

class Settings;
class WindowTeam;
class SkillsBar;
class CustomEntityManager;

using DialogEventObserver = ska::Observer<DialogEvent>;

class GUIMap :
	public ska::GUI,
	public ska::Observer<SettingsChangeEvent>,
	public ska::Observer<EntityLoadEvent>,
	public DialogEventObserver,
	public BattleStartObserver {

public:
	GUIMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged);
	GUIMap& operator=(const GUIMap&) = delete;

	void initButtons();
	bool onSettingsChange(SettingsChangeEvent& sce);
	bool onEntityLoad(EntityLoadEvent& ele);
	bool onBattleStart(BattleEvent& be);
	bool onDialogEvent(DialogEvent& de);
	virtual bool onScreenResized(unsigned int width, unsigned int height) override;
	void bind(Settings& sets);

	void refresh(unsigned int ellapsedTime);

	~GUIMap();

private:
	CustomEntityManager& m_entityManager;
	PokemonGameEventDispatcher& m_ged;
	WindowMouseCursor* m_attachedToCursor;
	WindowTeam* m_team;
	ska::TimeScrollableWindowIG<>* m_wAction;
	SkillsBar* m_skillBar;

	//Debug
	ska::DebugWindow m_dbgWindow;
	std::optional<ska::ScriptSleepComponent> m_dialogScript;
};
