#include "Physic/System/CollisionSystem.h"
#include "../PokemonGameEventDispatcher.h"
#include "../World/WorldState.h"
#include "../CustomEntityManager.h"
#include "StateToMapSwitcher.h"
#include "StateToBattleSwitcher.h"
#include "AbstractStateMap.h"
#include "StateMap.h"

AbstractStateMap::AbstractStateMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::StateHolder& sh, WorldState& ws, const bool sameMap) :
AbstractNoGUIStateMap(em, ged, w, ril, sh),
SceneChangeObserver(bind(&AbstractStateMap::onTeleport, this, std::placeholders::_1)),
m_sameMap(sameMap),
m_observersDefined(false),
m_worldScene(ws),
m_window(w) {
	m_collisionSystem = addLogic<ska::CollisionSystem>(ws.getWorld(), ged);
}

AbstractStateMap::AbstractStateMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, State& oldScene, WorldState& ws, const bool sameMap) :
AbstractNoGUIStateMap(em, ged, w, ril, oldScene),
SceneChangeObserver(bind(&AbstractStateMap::onTeleport, this, std::placeholders::_1)),
m_sameMap(sameMap),
m_observersDefined(true),
m_worldScene(ws),
m_window(w) {
	m_collisionSystem = addLogic<ska::CollisionSystem>(ws.getWorld(), ged);
}

void AbstractStateMap::beforeLoad(ska::ScenePtr*) {
	m_eventDispatcher.ska::Observable<MapEvent>::addObserver(*this);

	m_worldScene.linkCamera(&getCamera());
}

void AbstractStateMap::afterLoad(ska::ScenePtr*){
	/* If already loaded... */
	if (m_worldScene.loadedOnce()) {
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

bool AbstractStateMap::onTeleport(const MapEvent& me) {
	if(me.eventType == MapEvent::BATTLE) {
		StateToBattleSwitcher stbs(*me.fightComponent, me.fightPos, m_worldScene);
		stbs.switchTo(*this);
	} else {
		StateToMapSwitcher stms(me.mapName, me.chipsetName, m_worldScene);
		stms.switchTo(*this);
	}
	return true;
}

AbstractStateMap::~AbstractStateMap() {
	m_eventDispatcher.ska::Observable<MapEvent>::removeObserver(*this);
}
