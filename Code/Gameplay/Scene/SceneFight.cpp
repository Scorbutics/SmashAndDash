#include "../WGameCore.h"
#include "../Weather.h"
#include "../../ska/World/World.h"
#include "../../Graphic/Scrolling.h"
#include "../../ska/Script/ScriptDispatcher.h"
#include "SceneFight.h"


SceneFight::SceneFight()
{
}

void SceneFight::graphicUpdate(DrawableContainer& drawables) {
	 WGameCore& core = WGameCore::getInstance();
	/*World& world = core.getWorld();*/
	Pokeball& pokeball = core.getPokeball();
	Fight& fight = core.getFight();
	/*SpriteAnimationManager& spriteAnimManager = core.getSpriteAnimationManager();
	ParticleManager& particleManager = core.getParticleManager();
	MouseCursor& mouseCursor = core.getMouseCursor();
	GUI& gui = core.getGUI();
	Settings& settings = core.getSettings();*/

	AbstractSceneMap::graphicUpdate(drawables);
	
	//Affiche l'UI des combats et les attaques (disposé après le dessin de la Pokéball)
	fight.setPriority(pokeball.getPriority() + 1);
	drawables.add(fight);
}

void SceneFight::eventUpdate(bool movingDisallowed) {
	WGameCore& core = WGameCore::getInstance();
	Fight& fight = core.getFight();
	
	AbstractSceneMap::eventUpdate(movingDisallowed);
	
	//Evénements combat
	fight.refreshFight();
}

SceneFight::~SceneFight()
{
}
