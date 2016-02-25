#include "../WGameCore.h"
#include "../Weather.h"
#include "../World/World.h"
#include "../../Graphic/Scrolling.h"
#include "../../Script/ScriptDispatcher.h"
#include "SceneMap.h"
#include "../World/LayerE.h"

SceneMap::SceneMap()
{
}

void SceneMap::graphicUpdate(DrawableContainer& drawables) {
	/*WGameCore& core = WGameCore::getInstance();
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

void SceneMap::eventUpdate(bool stuck) {
	AbstractSceneMap::eventUpdate(stuck);

	WGameCore& core = WGameCore::getInstance();
	MobSpawningManager& mobSpawner = core.getMobSpawningManager();
	Fight& fight = core.getFight();

	//Syst�me de scripts
	//D�tection uniquement hors combats
	core.getWorld().getLayerEvent()->refresh();

	//g�re l'apparition al�atoire de Pok�mon	
	mobSpawner.refresh();

	//Ev�nements de lancement de combat	
	fight.refresh();

	/*WGameCore& core = WGameCore::getInstance();
	World& world = core.getWorld();
	Fight& fight = core.getFight();
	ParticleManager& particleManager = core.getParticleManager();
	GUI& gui = core.getGUI();
	MobSpawningManager& mobSpawner = core.getMobSpawningManager();
	ShakerManager& shaker = core.getShakerManager();
	InputListener& kdListener = core.getInputListener();*/
	
}

SceneMap::~SceneMap()
{
}
