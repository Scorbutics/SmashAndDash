#include "SceneGUIMap.h"
#include "Draw/DrawableContainer.h"

SceneGUIMap::SceneGUIMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::SceneHolder& sh) :
    SceneBase(em, ged, w, ril, sh),
    m_gui(w, ril, ged) {

}
void SceneGUIMap::onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {
	drawables.add(m_gui);
}

void SceneGUIMap::bindGUI(Settings& s) {
    m_gui.bind(s);
}

void SceneGUIMap::onEventUpdate(unsigned int ellapsedTime) {
	m_gui.refresh();
}
