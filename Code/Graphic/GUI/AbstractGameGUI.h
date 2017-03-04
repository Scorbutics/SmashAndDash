#pragma once

#include "../../ska/Graphic/GUI/GUI.h"
#include "../../Gameplay./PokemonGameEventDispatcher.h"

class AbstractGameGUI : public ska::GUI {
public:
	AbstractGameGUI(ska::Window& w, ska::InputContextManager& playerICM, PokemonGameEventDispatcher& ged);
	AbstractGameGUI& operator=(const AbstractGameGUI&) = delete;
	~AbstractGameGUI() = default;

protected:
	PokemonGameEventDispatcher& m_ged;
};