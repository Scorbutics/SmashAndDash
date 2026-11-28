#pragma once
#include <deque>
#include <functional>
#include <optional>
#include <string>
#include "Base/Patterns/SubObserver.h"
#include "Core/Data/Events/InputKeyEvent.h"
#include "Core/Data/Events/InputMouseEvent.h"
#include "Core/ECS/Basics/Script/ScriptSleepComponent.h"
#include "Core/Inputs/InputToggle.h"
#include "Canvas.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"

class CustomEntityManager;
class WorldState;

namespace ska {
	class DrawableContainer;
}

// The dialogs of the game (DialogEvent : the scripts, the start of a battle, an item found), drawn as the menus
// (Ui::Canvas) : a box at the bottom of the screen, the text written letter by letter, a blinking arrow when it is all
// written. Return writes it all at once, then closes it (a click too) ; a question : Oui / Non (A D or W S, Return),
// Escape or Backspace answers no. The dialogs that come while one is shown wait for it ; one that comes right after
// another takes its box (it does not slide out and in). While a dialog is shown, the trainer does not move nor act (its
// controls are held : HoldPlayerControls).
class DialogBox :
	public ska::SubObserver<DialogEvent>,
	public ska::SubObserver<ska::InputKeyEvent>,
	public ska::SubObserver<ska::InputMouseEvent> {
public:
	DialogBox(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& worldState);
	DialogBox(const DialogBox&) = delete;
	DialogBox& operator=(const DialogBox&) = delete;
	~DialogBox() override = default;

	void refresh(unsigned int ellapsedTime);
	void graphicUpdate(ska::DrawableContainer& drawables);

	// A letter every this long (ms), the box slides in this long, a dialog closed waits this long for the next one
	static constexpr unsigned int LetterTime = 18;
	static constexpr unsigned int SlideTime = 140;
	static constexpr unsigned int ChainDelay = 150;

private:
	struct Shown {
		std::string text;
		bool question = false;
		std::function<void(bool)> onClose;
		std::optional<ska::ScriptSleepComponent> script;
	};

	bool onDialog(DialogEvent& event);
	bool onKey(ska::InputKeyEvent& event);
	bool onMouse(ska::InputMouseEvent& event);
	void show(Shown dialog, bool chained);
	void next();
	void close(bool validated);
	std::size_t letters() const;
	bool written() const;
	void render(Ui::Canvas& canvas) const;
	ska::Rectangle box(int lines) const;

	CustomEntityManager& m_entities;
	PokemonGameEventDispatcher& m_ged;
	WorldState& m_worldState;
	Ui::Layer m_layer;

	std::optional<Shown> m_current;
	std::deque<Shown> m_waiting;
	// The text of the dialog, cut in lines ; its age ; written at once (Return)
	std::vector<std::string> m_lines;
	unsigned int m_age = 0;
	bool m_complete = false;
	bool m_chained = false;
	// A question : 0 yes, 1 no
	int m_answer = 0;
	// Closed : the box stays a moment (a next dialog takes it), then slides out
	std::optional<unsigned int> m_closedAge;
	std::vector<std::string> m_closedLines;
	unsigned int m_time = 0;
	// The trainer whose controls are held while a dialog is shown
	std::optional<ska::EntityId> m_heldTrainer;

	ska::InputToggleContainer m_lastToggles;
	bool m_lastConfirm = true;
	bool m_lastBack = true;
	bool m_lastClick = true;
	mutable ska::Rectangle m_box{};
	mutable ska::Rectangle m_yes{};
	mutable ska::Rectangle m_no{};
	const bool m_instant;
};
