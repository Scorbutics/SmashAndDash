#include "../WGameCore.h"
#include "../Weather.h"
#include "../../ska/World/World.h"
#include "../World/WorldScene.h"
#include "../../ska/World/LayerE.h"
#include "AbstractSceneMap.h"

AbstractSceneMap::AbstractSceneMap(WorldScene& ws, ska::SceneHolder& sh, ska::InputContextManager& ril, const bool sameMap) : 
AbstractNoGUISceneMap(sh, ril),
m_sameMap(sameMap),
m_worldScene(ws),
m_collisionSystem(ws.getWorld(), ws.getEntityManager()),
m_worldCollisionResponse(ws.getWorld(), m_collisionSystem, ws.getEntityManager()),
m_entityCollisionResponse(m_collisionSystem, ws.getEntityManager()) {
	m_logics.push_back(&m_collisionSystem);
}

AbstractSceneMap::AbstractSceneMap(WorldScene& ws, ska::Scene& oldScene, const bool sameMap) :
AbstractNoGUISceneMap(oldScene),
m_sameMap(sameMap),
m_worldScene(ws),
m_collisionSystem(ws.getWorld(), ws.getEntityManager()),
m_worldCollisionResponse(ws.getWorld(), m_collisionSystem, ws.getEntityManager()),
m_entityCollisionResponse(m_collisionSystem, ws.getEntityManager()) {
	m_logics.push_back(&m_collisionSystem);
}

void AbstractSceneMap::load(ska::ScenePtr* lastScene) {
	m_worldScene.linkCamera(&getCamera());
	bool firstTime = !m_worldScene.loadedOnce();
	if (!firstTime) {
		/* Do not delete the player between 2 maps, just TP it */
		std::unordered_set<ska::EntityId> toNotDelete;
		toNotDelete.insert(m_worldScene.getPlayer());

		if (!m_sameMap) {
			/* If the map changes, we delete all entities (except player) */
			m_worldScene.getEntityManager().removeEntities(toNotDelete);
		} else {
			m_worldScene.getEntityManager().refreshEntities();
		}
	}

}

bool AbstractSceneMap::unload() {
	return m_worldScene.unload();
}

void AbstractSceneMap::graphicUpdate(ska::DrawableContainer& drawables) {
	m_worldScene.graphicUpdate(drawables);
	AbstractNoGUISceneMap::graphicUpdate(drawables);

	WGameCore& core = WGameCore::getInstance();
	GUI& gui = core.getGUI();
	//Affiche la GUI
	drawables.add(gui);

	//Affiche le Pokémon ou l'objet sur le curseur de la souris
	//mouseCursor.displaySelectedPokemon();
	//mouseCursor.displaySelectedObject();

}

void AbstractSceneMap::eventUpdate(bool stuck) {
	WGameCore& core = WGameCore::getInstance();
	GUI& gui = core.getGUI();

	/* Raw input acquisition */
	m_inputCManager.refresh();

	AbstractNoGUISceneMap::eventUpdate(stuck);

	//GUI
	gui.dialogRefresh();
	gui.refresh();

	m_worldScene.eventUpdate(stuck);
}

AbstractSceneMap::~AbstractSceneMap()
{
}
