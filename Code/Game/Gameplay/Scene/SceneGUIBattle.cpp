#include "SceneGUIBattle.h"
#include "Draw/DrawableContainer.h"

SceneGUIBattle::SceneGUIBattle(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::SceneHolder& sh) :
    SceneBase(em, ged, w, ril, sh),
    m_gui(w, ril, ged) {

}
void SceneGUIBattle::onGraphicUpdate(ska::DrawableContainer& drawables) {
	//Affiche l'UI des combats et les attaques (disposé après le dessin de la Pokéball)
	m_gui.graphicUpdate(drawables);
}

bool SceneGUIBattle::beforeUnload() {
    /* Resets GUI */
    m_gui.clear();
}

void SceneGUIBattle::onEventUpdate(unsigned int ellapsedTime) {
	m_gui.eventUpdate(ellapsedTime);
}
