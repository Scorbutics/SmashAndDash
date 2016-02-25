#include "../WGameCore.h"
#include "../Weather.h"
#include "../World/World.h"
#include "../../Graphic/Scrolling.h"
#include "../../Script/ScriptDispatcher.h"
#include "../World/LayerE.h"
#include "AbstractSceneMap.h"

AbstractSceneMap::AbstractSceneMap()
{
}

void AbstractSceneMap::graphicUpdate(DrawableContainer& drawables) {
	WGameCore& core = WGameCore::getInstance();
	World& world = core.getWorld();
	Pokeball& pokeball = core.getPokeball();
	Fight& fight = core.getFight();
	SpriteAnimationManager& spriteAnimManager = core.getSpriteAnimationManager();
	ParticleManager& particleManager = core.getParticleManager();
	MouseCursor& mouseCursor = core.getMouseCursor();
	GUI& gui = core.getGUI();
	Settings& settings = core.getSettings();

	AbstractNoGUISceneMap::graphicUpdate(drawables);

	//Affiche les couches de blocs et les personnages
	world.graphicUpdate(drawables);

	//Affiche la pokeball si active
	drawables.addHead(pokeball);

	//Affiche l'UI des combats et les attaques
	drawables.addHead(fight);

	//Affiche les animations jouées
	drawables.addHead(spriteAnimManager);

	//Affiche les événements météorologiques
	/*particleManager.display(PARTICLE_MANAGER_CRUMBLING);
	particleManager.display(PARTICLE_MANAGER_RAIN);
	particleManager.displayRainFog();*/

	//Affiche la météo
	drawables.addHead(*world.getFog());
	drawables.addHead(*world.getWeather());

	//Affiche la GUI
	drawables.addHead(gui);

	//Affiche le Pokémon ou l'objet sur le curseur de la souris
	//mouseCursor.displaySelectedPokemon();
	//mouseCursor.displaySelectedObject();

}

void AbstractSceneMap::eventUpdate(bool stuck) {
	WGameCore& core = WGameCore::getInstance();
	GUI& gui = core.getGUI();
	InputListener& kdListener = core.getInputListener();
	
	AbstractNoGUISceneMap::eventUpdate(stuck);

	//Gère les évènements extérieurs de la GUI (input)
	//GUI A PASSER EN TANT QU'OBSERVATEUR
	if (!kdListener.refresh(stuck)) {
		gui.getDialog()->hide(true);
		gui.getImgDialog()->hide(true);
		gui.hide(false);
	}

	//GUI
	gui.dialogRefresh();
	gui.refresh();

}

AbstractSceneMap::~AbstractSceneMap()
{
}
