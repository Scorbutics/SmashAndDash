#include "StateGUIMap.h"
#include "Draw/DrawableContainer.h"

StateGUIMap::StateGUIMap(StateData& data, ska::StateHolder& sh) :
    StateBase(data.m_entityManager, data.m_eventDispatcher, sh),
    m_gui(data.m_eventDispatcher) {

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
