#include "Physic/System/CollisionSystem.h"
#include "../PokemonGameEventDispatcher.h"
#include "../World/WorldScene.h"
#include "../CustomEntityManager.h"
#include "SceneToMapSwitcher.h"
#include "SceneToBattleSwitcher.h"
#include "AbstractSceneMap.h"
#include "SceneMap.h"

AbstractSceneMap::AbstractSceneMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::SceneHolder& sh, WorldScene& ws, const bool sameMap) :
AbstractNoGUISceneMap(em, ged, w, ril, sh),
SceneChangeObserver(bind(&AbstractSceneMap::onTeleport, this, std::placeholders::_1)),
m_sameMap(sameMap), m_observersDefined(false),
	m_worldScene(ws),
m_window(w),
m_worldCollisionResponse(ws.getWorld(), ged, m_entityManager),
m_entityCollisionResponse(ged, m_entityManager) {
	
	m_collisionSystem = addLogic<ska::CollisionSystem>(ws.getWorld(), ged);
	m_eventDispatcher.ska::Observable<MapEvent>::addObserver(*this);
}

AbstractSceneMap::AbstractSceneMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, Scene& oldScene, WorldScene& ws, const bool sameMap) :
AbstractNoGUISceneMap(em, ged, w, ril, oldScene),
SceneChangeObserver(bind(&AbstractSceneMap::onTeleport, this, std::placeholders::_1)),
m_sameMap(sameMap),
m_worldScene(ws),
m_window(w),
m_worldCollisionResponse(ws.getWorld(), ged, m_entityManager),
m_entityCollisionResponse(ged, m_entityManager),
m_observersDefined(true) {
	m_collisionSystem = addLogic<ska::CollisionSystem>(ws.getWorld(), ged);
	
	m_eventDispatcher.ska::Observable<ska::CollisionEvent>::addObserver(m_entityCollisionResponse);
	m_eventDispatcher.ska::Observable<ska::CollisionEvent>::addObserver(m_worldCollisionResponse);
	m_eventDispatcher.ska::Observable<MapEvent>::addObserver(*this);

}

void AbstractSceneMap::load(ska::ScenePtr*) {
	if (m_observersDefined) {
		m_eventDispatcher.ska::Observable<ska::CollisionEvent>::addObserver(m_entityCollisionResponse);
		m_eventDispatcher.ska::Observable<ska::CollisionEvent>::addObserver(m_worldCollisionResponse);
	}
	m_eventDispatcher.ska::Observable<MapEvent>::addObserver(*this);

	m_worldScene.linkCamera(&getCamera());
	auto firstTime = !m_worldScene.loadedOnce();
	if (!firstTime) {
		/* Do not delete the player between 2 maps, just TP it */
		std::unordered_set<ska::EntityId> toNotDelete;
		toNotDelete.insert(m_worldScene.getPlayer());

		if (!m_sameMap) {
			/* If the map changes, we delete all entities (except player) */
			m_entityManager.removeEntities(toNotDelete);
		} else {
			m_entityManager.refreshEntities();
		}
	}

}

bool AbstractSceneMap::onTeleport(const MapEvent& me) {
	if(me.eventType == MapEvent::BATTLE) {
		SceneToBattleSwitcher stbs(*me.fightComponent, me.fightPos, m_worldScene);
		stbs.switchTo(*this);
	} else {
		SceneToMapSwitcher stms(me.mapName, me.chipsetName, m_worldScene);
		stms.switchTo(*this);
	}
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

void AbstractSceneMap::eventUpdate(unsigned int ellapsedTime) {
	/* Raw input acquisition */
	m_inputCManager.refresh();

	AbstractNoGUISceneMap::eventUpdate(ellapsedTime);
	m_worldScene.eventUpdate(ellapsedTime);
}

AbstractSceneMap::~AbstractSceneMap() {
	m_eventDispatcher.ska::Observable<MapEvent>::removeObserver(*this);
	if (m_observersDefined) {
		m_eventDispatcher.ska::Observable<ska::CollisionEvent>::removeObserver(m_worldCollisionResponse);
		m_eventDispatcher.ska::Observable<ska::CollisionEvent>::removeObserver(m_entityCollisionResponse);
	}
}
