#include "StateGUIMap.h"
#include "Draw/DrawableContainer.h"

StateGUIMap::StateGUIMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::StateHolder& sh) :
    StateBase(em, ged, w, ril, sh),
    m_gui(w, ril, ged) {

}
void StateGUIMap::onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {
	drawables.add(m_gui);
}

void StateGUIMap::bindGUI(Settings& s) {
    m_gui.bind(s);
}

void StateGUIMap::onEventUpdate(unsigned int ellapsedTime) {
	m_gui.refresh(ellapsedTime);
}
