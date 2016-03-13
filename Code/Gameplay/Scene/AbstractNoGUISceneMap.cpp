#include "AbstractNoGUISceneMap.h"
#include "../WGameCore.h"
#include "../Weather.h"
#include "../../ska/World/World.h"
#include "../../Graphic/Scrolling.h"
#include "../../ska/Script/ScriptDispatcher.h"
#include "../../ska/World/LayerE.h"

AbstractNoGUISceneMap::AbstractNoGUISceneMap(ska::RawInputListener& ril) : Scene(ril)
{
}

void AbstractNoGUISceneMap::graphicUpdate(ska::DrawableContainer& drawables) {
	WGameCore& core = WGameCore::getInstance();
	ska::World& world = core.getWorld();
	Pokeball& pokeball = core.getPokeball();
	Fight& fight = core.getFight();
	ska::SpriteAnimationManager& spriteAnimManager = core.getSpriteAnimationManager();
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
	/*drawables.addHead(*world.getFog());
	drawables.addHead(*world.getWeather());*/
}

void AbstractNoGUISceneMap::eventUpdate(bool movingDisallowed) {
	WGameCore& core = WGameCore::getInstance();
	ska::World& world = core.getWorld();
	Fight& fight = core.getFight();
	ska::ParticleManager& particleManager = core.getParticleManager();
	GUI& gui = core.getGUI();
	MobSpawningManager& mobSpawner = core.getMobSpawningManager();
	RainParticleManager& rainParticleManager = core.getRainParticleManager();
	ShakerManager& shaker = core.getShakerManager();
	//ska::InputListener& kdListener = core.getInputListener();

	m_inputCManager.refresh();

	world.refreshEntities();
	particleManager.refresh();
	rainParticleManager.refresh();

	//Syst�me de scripts
	//Rafra�chissement des scripts en cours d'ex�cution
	//NEW : dans "World" ?
	//ScriptDispatcher::getInstance().refresh();

	//Secoue l'�cran
	shaker.refresh();
}


AbstractNoGUISceneMap::~AbstractNoGUISceneMap()
{
}
