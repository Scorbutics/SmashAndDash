#include "AbstractNoGUISceneMap.h"
//#include "../Weather.h"
#include "World/World.h"


AbstractNoGUISceneMap::AbstractNoGUISceneMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::SceneHolder& sh) :
SceneBase(em, ged, w, ril, sh) {
}

AbstractNoGUISceneMap::AbstractNoGUISceneMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, Scene& oldScene) :
SceneBase(em, ged, w, ril, oldScene) {

}

void AbstractNoGUISceneMap::load(ska::ScenePtr*) {

}

bool AbstractNoGUISceneMap::unload() {
	return false;
}

void AbstractNoGUISceneMap::graphicUpdate(ska::DrawableContainer& drawables) {
	SceneBase::graphicUpdate(drawables);

	/*
	ska::World& world = core.getWorld();
	Pokeball& pokeball = core.getPokeball();
	Fight& fight = core.getFight();
	ska::SpriteAnimationManager& spriteAnimManager = core.getSpriteAnimationManager();
	RainParticleManager& rainParticleManager = core.getRainParticleManager();
	MouseCursor& mouseCursor = core.getMouseCursor();
	Settings& settings = core.getSettings();*/


	//Affiche la pokeball si active
	//drawables.addHead(pokeball);

	//Affiche les animations jouées
	/*spriteAnimManager.setPriority(pokeball.getPriority() + 5);
	drawables.add(spriteAnimManager);*/

	//Affiche les événements météorologiques à particules
	//drawables.addHead(rainParticleManager);

	//Affiche la météo
	/*drawables.addHead(*world.getFog());
	drawables.addHead(*world.getWeather());*/
}

void AbstractNoGUISceneMap::eventUpdate(unsigned int ellapsedTime) {
	SceneBase::eventUpdate(ellapsedTime);

	//WGameCore& core = WGameCore::getInstance();
	//ska::World& world = core.getWorld();
	//Fight& fight = core.getFight();
	//ska::ParticleManager& particleManager = core.getParticleManager();
	//GUI& gui = core.getGUI();
	//MobSpawningManager& mobSpawner = core.getMobSpawningManager();
	//RainParticleManager& rainParticleManager = core.getRainParticleManager();
	//ShakerManager& shaker = core.getShakerManager();
	//ska::InputListener& kdListener = core.getInputListener();



	/*world.refreshEntities();
	particleManager.refresh();
	rainParticleManager.refresh();*/


	//Secoue l'écran
	//shaker.refresh();


}


AbstractNoGUISceneMap::~AbstractNoGUISceneMap()
{
}
