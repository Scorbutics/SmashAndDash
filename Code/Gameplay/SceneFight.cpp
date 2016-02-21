#include "WGameCore.h"
#include "Weather.h"
#include "./World/World.h"
#include "GestionEvents.h"
#include "../Graphic/Scrolling.h"
#include "../Script/ScriptDispatcher.h"
#include "SceneFight.h"


SceneFight::SceneFight()
{
}

void SceneFight::graphicUpdate(void) {
	WGameCore& core = WGameCore::getInstance();
	World& world = core.getWorld();
	Pokeball& pokeball = core.getPokeball();
	Fight& fight = core.getFight();
	SpriteAnimationManager& spriteAnimManager = core.getSpriteAnimationManager();
	ParticleManager& particleManager = core.getParticleManager();
	MouseCursor& mouseCursor = core.getMouseCursor();
	GUI& gui = core.getGUI();
	Settings& settings = core.getSettings();

	//Affiche les couches de blocs et les personnages
	world.displayLayers();

	//Affiche la pokeball si active
	pokeball.refresh();

	//Affiche l'UI des combats et les attaques
	fight.display();

	//Affiche les animations jouées
	spriteAnimManager.refresh();

	//Affiche les événements météorologiques
	particleManager.display(PARTICLE_MANAGER_CRUMBLING);
	particleManager.display(PARTICLE_MANAGER_RAIN);
	particleManager.displayRainFog();

	//Affiche la météo
	if (world.getFog()->isActive() && *settings.getFogActive()) {
		world.getFog()->display();
	}

	if (world.getWeather()->isActive() && *settings.getWeatherActive()) {
		world.getWeather()->display();
	}

	//Affiche la GUI
	gui.dialogDisplay();
	if (gui.isVisible()) {
		gui.display();
	}

	//Euh...
	world.setBgmVolume(*settings.getSoundVolume());

	//Affiche le Pokémon ou l'objet sur le curseur de la souris
	mouseCursor.displaySelectedPokemon();
	mouseCursor.displaySelectedObject();

	/* TODO faire une gestion de caméra externe (non uniquement focus sur le héro) */
	Scrolling();
}

void SceneFight::eventUpdate(bool movingDisallowed) {
	AbstractSceneMap::eventUpdate(movingDisallowed);

	
}

SceneFight::~SceneFight()
{
}
