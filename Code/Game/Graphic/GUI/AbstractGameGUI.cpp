#include "AbstractGameGUI.h"
#include "Core/Window.h"

AbstractGameGUI::AbstractGameGUI(ska::Window& w, ska::InputContextManager& playerICM, PokemonGameEventDispatcher& ged) :
	GUI(ged, w, playerICM),
	m_ged(ged) {

}

