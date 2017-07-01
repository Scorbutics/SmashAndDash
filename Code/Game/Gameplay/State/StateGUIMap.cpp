#include "StateGUIMap.h"
#include "Draw/DrawableContainer.h"

StateGUIMap::StateGUIMap(StateData& data, ska::StateHolder& sh) :
    StateBase(data.m_entityManager, data.m_eventDispatcher, data.m_window, data.m_inputCManager, sh),
    m_gui(data.m_window, data.m_inputCManager, data.m_eventDispatcher) {

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
