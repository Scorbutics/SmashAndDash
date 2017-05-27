#include "SceneGUIBattle.h"
#include "Draw/DrawableContainer.h"

SceneGUIBattle::SceneGUIBattle(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::SceneHolder& sh) :
    SceneBase(em, ged, w, ril, sh),
    m_gui(w, ril, ged) {

}
void SceneGUIBattle::onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {
	//Affiche l'UI des combats et les attaques (dispos� apr�s le dessin de la Pok�ball)
	m_gui.graphicUpdate(ellapsedTime, drawables);
}

bool SceneGUIBattle::beforeUnload() {
    /* Resets GUI */
    m_gui.clear();
	return false;
}

void SceneGUIBattle::onEventUpdate(unsigned int ellapsedTime) {
	m_gui.eventUpdate(ellapsedTime);
}
