#include "StateGUIBattle.h"
#include "Draw/DrawableContainer.h"

StateGUIBattle::StateGUIBattle(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::StateHolder& sh) :
    StateBase(em, ged, w, ril, sh),
    m_gui(w, ril, ged) {

}
void StateGUIBattle::onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {
	//Affiche l'UI des combats et les attaques (disposé après le dessin de la Pokéball)
	m_gui.graphicUpdate(ellapsedTime, drawables);
}

bool StateGUIBattle::beforeUnload() {
    /* Resets GUI */
    m_gui.clear();
	return false;
}

void StateGUIBattle::onEventUpdate(unsigned int ellapsedTime) {
	//m_gui.eventUpdate(ellapsedTime);
}
