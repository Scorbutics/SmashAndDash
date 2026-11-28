#pragma once
#include <map>
#include <optional>
#include <string>
#include <vector>
#include "Base/Patterns/SubObserver.h"
#include "Core/Data/Events/InputKeyEvent.h"
#include "Core/Data/Events/InputMouseEvent.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/Inputs/InputToggle.h"
#include "Graphic/Texture.h"
#include "Canvas.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"

class WorldState;
class CustomEntityManager;
class Settings;

namespace ska {
	class DrawableContainer;
}

// The menu of the trainer on the map, drawn as the battles (Ui::Canvas, the theme of Menu/default_theme), with the keys
// (the pad of the web page) or the mouse. Three cards at the bottom of the screen (the team, the bag, the options : a
// click opens them) ; Tab or M (Menu) opens the menu, Escape or Backspace goes back. W S A D move, Return chooses.
//   Équipe   the Pokemon of the team (hit points, level, types) ; a Pokemon : its summary (its skills), or it is moved
//            to another place (the first one able to fight follows the player and fights)
//   Sac      the items, their description, the money ; a healing item on a Pokemon of the team
//   Options  the volumes (everything, the music, the sounds), the weather, the fog (Settings : saved)
// Open, the trainer does not move nor act (its controls are held : HoldPlayerControls). It also draws the choices asked by the scripts
// (GameDialog.select : WorldState::prompt), on the map and in the battles.
class TrainerMenu :
	public ska::SubObserver<ska::InputKeyEvent>,
	public ska::SubObserver<ska::InputMouseEvent>,
	public ska::SubObserver<InterfaceEvent> {
public:
	TrainerMenu(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& worldState, Settings& settings);
	TrainerMenu(const TrainerMenu&) = delete;
	TrainerMenu& operator=(const TrainerMenu&) = delete;
	~TrainerMenu() override = default;

	void refresh(unsigned int ellapsedTime);
	void graphicUpdate(ska::DrawableContainer& drawables);

private:
	enum class Mode { Closed, Tabs, Team, TeamActions, Summary, Moving, Bag, BagTarget, Options };
	struct Member {
		int index = 0;
		int id = 0;
		std::string name;
		int level = 1;
		int hp = 0;
		int maxHp = 1;
		float experience = 0.F;
		std::string type1;
		std::string type2;
		std::vector<std::string> skills;
		std::vector<std::string> skillTypes;
	};
	struct Item {
		int id = 0;
		std::string name;
		std::string description;
		int amount = 0;
		bool heal = false;
		bool capture = false;
	};
	// A part of the screen the mouse acts on (drawn last frame)
	struct Hit {
		ska::Rectangle box;
		Mode mode;
		int index;
	};

	bool onKey(ska::InputKeyEvent& event);
	bool onMouse(ska::InputMouseEvent& event);
	bool onInterface(InterfaceEvent& event);

	bool available() const;
	void open(Mode mode);
	void close();
	// The team, the bag (read again : after a change, at the opening)
	void reload();
	void notice(std::string text);
	// A direction (W S A D) or a choice (Return), a back (Escape, Backspace)
	void move(int dx, int dy);
	void confirm();
	void back();
	void option(int row, int direction);
	void useItem(int teamIndex);

	void render(Ui::Canvas& canvas) const;
	void renderCards(Ui::Canvas& canvas) const;
	void renderTabs(Ui::Canvas& canvas, const ska::Rectangle& area) const;
	void renderTeam(Ui::Canvas& canvas, const ska::Rectangle& area) const;
	void renderSummary(Ui::Canvas& canvas, const ska::Rectangle& area) const;
	void renderBag(Ui::Canvas& canvas, const ska::Rectangle& area) const;
	void renderOptions(Ui::Canvas& canvas, const ska::Rectangle& area) const;
	void renderPrompt(Ui::Canvas& canvas) const;
	void hint(Ui::Canvas& canvas, const std::string& text) const;
	// A Pokemon from the front (its character sheet), its feet at x y
	void pokemon(Ui::Canvas& canvas, int id, int x, int y, float scale) const;
	const ska::Texture& charset(int id) const;
	ska::Point<int> screen() const;

	CustomEntityManager& m_entities;
	WorldState& m_worldState;
	Settings& m_settings;
	Ui::Layer m_layer;

	Mode m_mode = Mode::Closed;
	int m_tab = 0;
	int m_teamCursor = 0;
	int m_actionCursor = 0;
	int m_movingFrom = -1;
	int m_bagCursor = 0;
	int m_targetCursor = 0;
	int m_optionCursor = 0;
	unsigned int m_time = 0;
	unsigned int m_openTime = 0;
	bool m_hidden = false;
	std::string m_notice;
	unsigned int m_noticeAge = 0;

	std::vector<Member> m_team;
	std::vector<Item> m_items;
	int m_money = 0;
	// The first Pokemon of the team (the card of the team), read from time to time
	int m_leadId = -1;
	unsigned int m_leadCheck = 1500;

	// The trainer whose controls the menu holds while it is open
	std::optional<ska::EntityId> m_heldTrainer;

	// The keys of last frame (a key acts when it is pressed), the mouse
	ska::InputToggleContainer m_lastToggles;
	bool m_lastConfirm = true;
	bool m_lastBack = true;
	bool m_lastMenu = true;
	bool m_lastClick = true;
	Ui::ChoiceList m_promptChoice;
	bool m_promptShown = false;
	mutable std::vector<Hit> m_hits;
	mutable std::map<int, ska::Texture> m_charsets;
	ska::Texture m_ball;
	mutable ska::Point<int> m_screen{};
};
