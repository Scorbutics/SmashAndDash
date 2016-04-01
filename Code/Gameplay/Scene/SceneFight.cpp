#include "../World/WorldScene.h"
#include "SceneFight.h"
#include "../CustomEntityManager.h"

SceneFight::SceneFight(ska::SceneHolder& sh, WorldScene& ws, ska::InputContextManager& ril, ska::Point<int> fightPos) :
AbstractSceneMap(sh, ril),
m_worldScene(ws),
m_cameraSystem(ws.getEntityManager(), ws.getScreenW(), ws.getScreenH(), fightPos) {
	m_logics.push_back(&m_cameraSystem);
}

void SceneFight::graphicUpdate(ska::DrawableContainer& drawables) {
	 //WGameCore& core = WGameCore::getInstance();
	/*World& world = core.getWorld();*/
	/*Pokeball& pokeball = core.getPokeball();
	Fight& fight = core.getFight();*/
	/*SpriteAnimationManager& spriteAnimManager = core.getSpriteAnimationManager();
	ParticleManager& particleManager = core.getParticleManager();
	MouseCursor& mouseCursor = core.getMouseCursor();
	GUI& gui = core.getGUI();
	Settings& settings = core.getSettings();*/

	m_worldScene.graphicUpdate(drawables);
	AbstractSceneMap::graphicUpdate(drawables);
	
	//Affiche l'UI des combats et les attaques (disposé après le dessin de la Pokéball)
	/*fight.setPriority(pokeball.getPriority() + 1);*/
	/*drawables.add(fight);*/
}

void SceneFight::load() {
	m_worldScene.linkCamera(&m_cameraSystem);
	m_worldScene.load();
}

void SceneFight::unload() {
	m_worldScene.unload();
}

void SceneFight::eventUpdate(bool movingDisallowed) {
	/*WGameCore& core = WGameCore::getInstance();
	Fight& fight = core.getFight();*/
	m_worldScene.eventUpdate(movingDisallowed);
	AbstractSceneMap::eventUpdate(movingDisallowed);
	
	//Evénements combat
	/*fight.refreshFight();*/
}

SceneFight::~SceneFight()
{
}
