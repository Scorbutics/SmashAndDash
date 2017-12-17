#include "Physic/System/CollisionSystem.h"
#include "../PokemonGameEventDispatcher.h"
#include "../World/WorldState.h"
#include "StateToMapSwitcher.h"
#include "StateToBattleSwitcher.h"
#include "AbstractStateMap.h"
#include "StateMap.h"
#include "Physic/System/WorldCollisionSystem.h"

AbstractStateMap::AbstractStateMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged) :
StateBase(em),
StateChangeObserver(bind(&AbstractStateMap::onTeleport, this, std::placeholders::_1), ged),
m_observersDefined(false),
m_eventDispatcher(ged),
m_entityManager(em) {
	//m_collisionSystem = addLogic<ska::CollisionSystem>(ged);
	//addLogic<ska::WorldCollisionSystem>(ws.getWorld(), ged);
}

void AbstractStateMap::beforeLoad(ska::StatePtr*) {

	//m_worldState.linkCamera(&getCamera());
}

void AbstractStateMap::afterLoad(ska::StatePtr*) {
	/* If already loaded... */
	/*if (m_worldState.loadedOnce()) {
		// Do not delete the player between 2 maps, just TP it
		std::unordered_set<ska::EntityId> toNotDelete;
		toNotDelete.insert(m_worldState.getPlayer());

		if (!m_sameMap) {
			// If the map changes, we delete all entities (except player)
			m_entityManager.removeEntities(toNotDelete);
		} else {
			m_entityManager.refreshEntities();
		}
	}
	*/
}

bool AbstractStateMap::onTeleport(const MapEvent& me) {
	/*if(me.eventType == MapEvent::BATTLE) {
		m_holder.nextState<StateFight>(me.fightComponent, me.fightPos, m_worldState, getCamera().getScreenSize());
	} else {
		m_holder.nextState<StateMap>(".\\Levels\\" + me.mapName + ".bmp", me.chipsetName, m_worldState, getCamera().getScreenSize());
	}*/
	return true;
}

AbstractStateMap::~AbstractStateMap() {
}
