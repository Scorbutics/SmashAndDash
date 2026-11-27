#include "Game/__internalConfig/LoggerConfig.h"
#include "StateGUIBattle.h"
#include "Core/Draw/DrawableContainer.h"

StateGUIBattle::StateGUIBattle(CustomEntityManager& em, BattleStage& stage, const BattleSystem& battle, ska::CameraSystem& camera) :
	m_gui(em, stage, battle, camera) {

}
void StateGUIBattle::onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {
	//Affiche l'UI des combats et les attaques (disposé après le dessin de la Pokéball)
	m_gui.graphicUpdate(ellapsedTime, drawables);
}

void StateGUIBattle::beforeUnload() {
}

void StateGUIBattle::onEventUpdate(unsigned int) {
	//m_gui.eventUpdate(ellapsedTime);
}
