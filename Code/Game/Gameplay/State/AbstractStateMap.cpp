#include "Physic/System/CollisionSystem.h"
#include "../PokemonGameEventDispatcher.h"
#include "../World/WorldState.h"
#include "StateToMapSwitcher.h"
#include "StateToBattleSwitcher.h"
#include "AbstractStateMap.h"
#include "Physic/System/WorldCollisionSystem.h"

AbstractStateMap::AbstractStateMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& ws) :
	m_observersDefined(false),
	m_eventDispatcher(ged),
	m_entityManager(em), 
	m_collisionSystem(nullptr),
	m_worldState(ws) {
	m_worldState.linkCamera(nullptr);
}

void AbstractStateMap::beforeLoad(ska::StatePtr*) {
	m_collisionSystem = addLogic<ska::CollisionSystem>(m_entityManager, m_eventDispatcher);
	addLogic<ska::WorldCollisionSystem>(m_entityManager, m_worldState.getWorld(), m_eventDispatcher);
}

void AbstractStateMap::afterLoad(ska::StatePtr*) {
	m_worldState.linkCamera(getCamera());
}

AbstractStateMap::~AbstractStateMap() {
}
