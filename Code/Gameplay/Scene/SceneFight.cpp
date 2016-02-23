#include "../WGameCore.h"
#include "../Weather.h"
#include "../World/World.h"
#include "../../Graphic/Scrolling.h"
#include "../../Script/ScriptDispatcher.h"
#include "SceneFight.h"


SceneFight::SceneFight()
{
}

void SceneFight::graphicUpdate(std::priority_queue<Drawable*>& drawables) {
	/* WGameCore& core = WGameCore::getInstance();
	World& world = core.getWorld();
	Pokeball& pokeball = core.getPokeball();
	Fight& fight = core.getFight();
	SpriteAnimationManager& spriteAnimManager = core.getSpriteAnimationManager();
	ParticleManager& particleManager = core.getParticleManager();
	MouseCursor& mouseCursor = core.getMouseCursor();
	GUI& gui = core.getGUI();
	Settings& settings = core.getSettings();*/

	AbstractSceneMap::graphicUpdate(drawables);
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
