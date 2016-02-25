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

}

void AbstractNoGUISceneMap::eventUpdate(bool movingDisallowed) {
	WGameCore& core = WGameCore::getInstance();
	World& world = core.getWorld();
	Fight& fight = core.getFight();
	ParticleManager& particleManager = core.getParticleManager();
	GUI& gui = core.getGUI();
	MobSpawningManager& mobSpawner = core.getMobSpawningManager();
	ShakerManager& shaker = core.getShakerManager();
	InputListener& kdListener = core.getInputListener();

	world.refreshEntities();
	particleManager.refresh(PARTICLE_MANAGER_EFFECT);
	particleManager.refresh(PARTICLE_MANAGER_CRUMBLING);
	particleManager.refresh(PARTICLE_MANAGER_RAIN);


	//Syst�me de scripts
	//Rafra�chissement des scripts en cours d'ex�cution
	ScriptDispatcher::getInstance().refresh();

	//Secoue l'�cran
	shaker.refresh();
}


AbstractNoGUISceneMap::~AbstractNoGUISceneMap()
{
}
