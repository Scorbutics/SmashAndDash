#include "WGameCore.h"
#include "Weather.h"
#include "./World/World.h"
#include "GestionEvents.h"
#include "../Graphic/Scrolling.h"
#include "../Script/ScriptDispatcher.h"
#include "SceneMap.h"

SceneMap::SceneMap()
{
}

void SceneMap::graphicUpdate(void) {
	/*WGameCore& core = WGameCore::getInstance();
	World& world = core.getWorld();
	Pokeball& pokeball = core.getPokeball();
	Fight& fight = core.getFight();
	SpriteAnimationManager& spriteAnimManager = core.getSpriteAnimationManager();
	ParticleManager& particleManager = core.getParticleManager();
	MouseCursor& mouseCursor = core.getMouseCursor();
	GUI& gui = core.getGUI();
	Settings& settings = core.getSettings();*/

	AbstractSceneMap::graphicUpdate();
}

void SceneMap::eventUpdate(bool stuck) {
	AbstractSceneMap::eventUpdate(stuck);

	WGameCore& core = WGameCore::getInstance();
	MobSpawningManager& mobSpawner = core.getMobSpawningManager();

	//gère l'apparition aléatoire de Pokémon	
	mobSpawner.refresh();

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
