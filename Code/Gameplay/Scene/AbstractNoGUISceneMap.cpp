#include "AbstractNoGUISceneMap.h"
#include "../WGameCore.h"
#include "../Weather.h"
#include "../World/World.h"
#include "../../Graphic/Scrolling.h"
#include "../../Script/ScriptDispatcher.h"
#include "../World/LayerE.h"

AbstractNoGUISceneMap::AbstractNoGUISceneMap()
{
}

void AbstractNoGUISceneMap::graphicUpdate(DrawableContainer& drawables) {
	WGameCore& core = WGameCore::getInstance();
	World& world = core.getWorld();
	Pokeball& pokeball = core.getPokeball();
	Fight& fight = core.getFight();
	SpriteAnimationManager& spriteAnimManager = core.getSpriteAnimationManager();
	RainParticleManager& rainParticleManager = core.getRainParticleManager();
	MouseCursor& mouseCursor = core.getMouseCursor();
	Settings& settings = core.getSettings();

	//Affiche les couches de blocs et les personnages
	world.graphicUpdate(drawables);

	//Affiche la pokeball si active
	drawables.addHead(pokeball);

	//Affiche les animations jou�es
	spriteAnimManager.setPriority(pokeball.getPriority() + 5);
	drawables.add(spriteAnimManager);

	//Affiche les �v�nements m�t�orologiques � particules
	drawables.addHead(rainParticleManager);

	//Affiche la m�t�o
	drawables.addHead(*world.getFog());
	drawables.addHead(*world.getWeather());
}

void AbstractNoGUISceneMap::eventUpdate(bool movingDisallowed) {
	WGameCore& core = WGameCore::getInstance();
	World& world = core.getWorld();
	Fight& fight = core.getFight();
	ParticleManager& particleManager = core.getParticleManager();
	GUI& gui = core.getGUI();
	MobSpawningManager& mobSpawner = core.getMobSpawningManager();
	RainParticleManager& rainParticleManager = core.getRainParticleManager();
	ShakerManager& shaker = core.getShakerManager();
	InputListener& kdListener = core.getInputListener();

	world.refreshEntities();
	particleManager.refresh();
	rainParticleManager.refresh();
	/*particleManager.refresh(PARTICLE_MANAGER_EFFECT);
	particleManager.refresh(PARTICLE_MANAGER_CRUMBLING);
	particleManager.refresh(PARTICLE_MANAGER_RAIN);*/


	//Syst�me de scripts
	//Rafra�chissement des scripts en cours d'ex�cution
	ScriptDispatcher::getInstance().refresh();

	//Secoue l'�cran
	shaker.refresh();
}


AbstractNoGUISceneMap::~AbstractNoGUISceneMap()
{
}
