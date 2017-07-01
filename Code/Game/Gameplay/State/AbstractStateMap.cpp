#include "Physic/System/CollisionSystem.h"
#include "../PokemonGameEventDispatcher.h"
#include "../World/WorldState.h"
#include "../CustomEntityManager.h"
#include "StateToMapSwitcher.h"
#include "StateToBattleSwitcher.h"
#include "AbstractStateMap.h"
#include "StateMap.h"

AbstractStateMap::AbstractStateMap(StateData& data, ska::StateHolder& sh, WorldState& ws, const bool sameMap) :
StateBase(data.m_entityManager, data.m_eventDispatcher, data.m_window, data.m_inputCManager, sh),
StateChangeObserver(bind(&AbstractStateMap::onTeleport, this, std::placeholders::_1)),
m_sameMap(sameMap),
m_observersDefined(false),
m_eventDispatcher(data.m_eventDispatcher),
m_entityManager(data.m_entityManager),
m_worldState(ws),
m_window(data.m_window) {
	m_collisionSystem = addLogic<ska::CollisionSystem>(ws.getWorld(), data.m_eventDispatcher);
}

AbstractStateMap::AbstractStateMap(StateData& data, State& oldScene, WorldState& ws, const bool sameMap) :
StateBase(data.m_entityManager, data.m_eventDispatcher, data.m_window, data.m_inputCManager, oldScene),
StateChangeObserver(bind(&AbstractStateMap::onTeleport, this, std::placeholders::_1)),
m_sameMap(sameMap),
m_observersDefined(true),
m_eventDispatcher(data.m_eventDispatcher),
m_entityManager(data.m_entityManager),
m_worldState(ws),
m_window(data.m_window) {
	m_collisionSystem = addLogic<ska::CollisionSystem>(ws.getWorld(), data.m_eventDispatcher);
}

void AbstractStateMap::beforeLoad(ska::StatePtr*) {
	m_eventDispatcher.ska::Observable<MapEvent>::addObserver(*this);

	m_worldState.linkCamera(&getCamera());
}

void AbstractStateMap::afterLoad(ska::StatePtr*){
	/* If already loaded... */
	if (m_worldState.loadedOnce()) {
		/* Do not delete the player between 2 maps, just TP it */
		std::unordered_set<ska::EntityId> toNotDelete;
		toNotDelete.insert(m_worldState.getPlayer());

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
		StateToBattleSwitcher stbs(*me.fightComponent, me.fightPos, m_worldState);
		stbs.switchTo(*this);
	} else {
		StateToMapSwitcher stms(me.mapName, me.chipsetName, m_worldState);
		stms.switchTo(*this);
	}
	return true;
}

AbstractStateMap::~AbstractStateMap() {
	m_eventDispatcher.ska::Observable<MapEvent>::removeObserver(*this);
}
