#include "../Weather.h"
#include "../World/WorldScene.h"
#include "../CustomEntityManager.h"
#include "../../ska/World/LayerE.h"
#include "../../ska/Scene/SceneSwitcher.h"
#include "AbstractSceneMap.h"

AbstractSceneMap::AbstractSceneMap(ska::Window& w, WorldScene& ws, ska::SceneHolder& sh, ska::InputContextManager& ril, const bool sameMap) :
AbstractNoGUISceneMap(sh, ril),
SceneChangeObserver(std::bind(&AbstractSceneMap::onTeleport, this, std::placeholders::_1)),
m_sameMap(sameMap),
m_worldScene(ws),
m_collisionSystem(ws.getWorld(), ws.getEntityManager()),
m_worldCollisionResponse(ws.getWorld(), m_collisionSystem, ws.getEntityManager()),
m_entityCollisionResponse(m_collisionSystem, ws.getEntityManager()),
m_window(w) {
	m_logics.push_back(&m_collisionSystem);
}

AbstractSceneMap::AbstractSceneMap(ska::Window& w, WorldScene& ws, ska::Scene& oldScene, const bool sameMap) :
AbstractNoGUISceneMap(oldScene),
SceneChangeObserver(std::bind(&AbstractSceneMap::onTeleport, this, std::placeholders::_1)),
m_sameMap(sameMap),
m_worldScene(ws),
m_collisionSystem(ws.getWorld(), ws.getEntityManager()),
m_worldCollisionResponse(ws.getWorld(), m_collisionSystem, ws.getEntityManager()),
m_entityCollisionResponse(m_collisionSystem, ws.getEntityManager()),
m_window(w) {
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

bool AbstractSceneMap::onTeleport(const ska::SceneSwitcher<WorldScene&>& switcher) {
	switcher.switchTo(m_window, m_holder, *this, m_inputCManager, m_worldScene);
	return true;
}

bool AbstractSceneMap::unload() {
	return m_worldScene.unload();
}

void AbstractSceneMap::graphicUpdate(ska::DrawableContainer& drawables) {
	m_worldScene.graphicUpdate(drawables);
	AbstractNoGUISceneMap::graphicUpdate(drawables);

	//Affiche le Pokémon ou l'objet sur le curseur de la souris
	//mouseCursor.displaySelectedPokemon();
	//mouseCursor.displaySelectedObject();

}

void AbstractSceneMap::eventUpdate(bool stuck) {
	/* Raw input acquisition */
	m_inputCManager.refresh();

	AbstractNoGUISceneMap::eventUpdate(stuck);
	m_worldScene.eventUpdate(stuck);
}

AbstractSceneMap::~AbstractSceneMap()
{
}
