#include "StateGUIBattle.h"
#include "Draw/DrawableContainer.h"

StateGUIBattle::StateGUIBattle(StateData& data, ska::StateHolder& sh) :
    StateBase(data.m_entityManager, data.m_eventDispatcher, data.m_window, data.m_inputCManager, sh),
    m_gui(data.m_window, data.m_inputCManager, data.m_eventDispatcher) {

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
