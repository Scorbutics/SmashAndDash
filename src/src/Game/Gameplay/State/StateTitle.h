#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "Core/State/StateBase.h"
#include "../../Graphic/GUI/Canvas.h"
#include "Core/Data/Events/InputKeyEvent.h"
#include "Core/Data/Events/InputMouseEvent.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/Data/Events/GameEvent.h"
#include "Core/Point.h"
#include "Base/Patterns/SubObserver.h"
#include "Graphic/Texture.h"
#include "../PokemonGameEventDispatcher.h"

namespace worldgen {
	class InfiniteWorldWriter;
}

// The title screen : continue a save (the most recent first) or start a new game ; the loading screen of a world being
// generated (a step of the generator per frame : the progress is drawn, on the web too). Then the game plays the save
// (start). Keys : up / down (W S, the arrows), Return or Space ; the mouse.
class StateTitle :
	public ska::StateBase,
	public ska::SubObserver<ska::InputKeyEvent>,
	public ska::SubObserver<ska::InputMouseEvent>,
	public ska::SubObserver<ska::GameEvent> {
public:
	// direct : a save to play at once (generated first when its world is to generate), else the menu
	StateTitle(PokemonGameEventDispatcher& ged, std::function<void(const std::string&)> start, const std::string& direct = "");
	~StateTitle() override;

protected:
	void onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) override;

private:
	struct Entry {
		std::string label;
		std::string detail;
		std::string save; // empty : a new game
	};
	enum class Mode { Menu, Loading, Started, Failed };

	bool onKey(ska::InputKeyEvent& event);
	bool onMouse(ska::InputMouseEvent& event);
	bool onGameEvent(ska::GameEvent& event);
	void choose(const Entry& entry);
	void play(const std::string& save);
	ska::Rectangle entryBox(std::size_t index) const;

	void render(Ui::Canvas& canvas) const;
	void renderBackground(Ui::Canvas& canvas) const;
	void renderMenu(Ui::Canvas& canvas) const;
	void renderLoading(Ui::Canvas& canvas) const;

	std::function<void(const std::string&)> m_start;
	Mode m_mode = Mode::Menu;
	std::vector<Entry> m_entries;
	Ui::ChoiceList m_choices;

	std::string m_save;
	long long m_seed = 0;
	std::unique_ptr<worldgen::InfiniteWorldWriter> m_writer;
	int m_framesShown = 0;
	std::string m_error;

	ska::Point<int> m_screen{ 1120, 640 };
	unsigned int m_time = 0;
	ska::Texture m_hero;
	Ui::Layer m_layer;
};
