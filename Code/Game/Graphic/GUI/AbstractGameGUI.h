#pragma once

#include "GUI/GUI.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"

namespace ska {
	class Window;
}

class AbstractGameGUI : public ska::GUI {
public:
	AbstractGameGUI(ska::Window& w, ska::InputContextManager& playerICM, PokemonGameEventDispatcher& ged);
	AbstractGameGUI& operator=(const AbstractGameGUI&) = delete;
	~AbstractGameGUI() = default;

protected:
	PokemonGameEventDispatcher& m_ged;
};
