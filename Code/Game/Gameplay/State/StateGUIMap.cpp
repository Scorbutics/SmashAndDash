#include "StateGUIMap.h"
#include "Draw/DrawableContainer.h"

StateGUIMap::StateGUIMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged) :
    StateBase(em),
    m_gui(ged) {

}
void StateGUIMap::onGraphicUpdate(unsigned int, ska::DrawableContainer& drawables) {
	drawables.add(m_gui);
}

void StateGUIMap::bindGUI(Settings& s) {
    m_gui.bind(s);
}

void StateGUIMap::onEventUpdate(unsigned int ellapsedTime) {
	m_gui.refresh(ellapsedTime);
}
