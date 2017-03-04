#include "AbstractGameGUI.h"

AbstractGameGUI::AbstractGameGUI(ska::Window& w, ska::InputContextManager& playerICM, PokemonGameEventDispatcher& ged) :
	GUI(w, playerICM),
	m_ged(ged) {

}

