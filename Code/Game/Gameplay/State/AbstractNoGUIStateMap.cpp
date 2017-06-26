#include "AbstractNoGUIStateMap.h"
//#include "../Weather.h"
#include "World/World.h"


AbstractNoGUIStateMap::AbstractNoGUIStateMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::StateHolder& sh) :
	StateBase(em, ged, w, ril, sh) {
}

AbstractNoGUIStateMap::AbstractNoGUIStateMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, State& oldScene) :
	StateBase(em, ged, w, ril, oldScene) {

}

void AbstractNoGUIStateMap::onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {
	//StateBase::graphicUpdate(drawables);

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

void AbstractNoGUIStateMap::onEventUpdate(unsigned int ellapsedTime) {
	//StateBase::eventUpdate(ellapsedTime);

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


AbstractNoGUIStateMap::~AbstractNoGUIStateMap()
{
}
