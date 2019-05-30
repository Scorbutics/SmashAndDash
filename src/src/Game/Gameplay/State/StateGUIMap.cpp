#include "Game/__internalConfig/LoggerConfig.h"
#include "StateGUIMap.h"
#include "Core/Draw/DrawableContainer.h"

StateGUIMap::StateGUIMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged) :
    m_gui(em, ged) {

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
