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

void AbstractStateMap::beforeLoad(ska::State*) {
	auto colSys = std::make_unique<ska::CollisionSystem>(m_entityManager, m_eventDispatcher);
	m_collisionSystem = colSys.get();
	addLogic(std::move(colSys));
	addLogic(std::make_unique<ska::WorldCollisionSystem>(m_entityManager, m_worldState.getWorld(), m_eventDispatcher));
}

void AbstractStateMap::afterLoad(ska::State*) {
	m_worldState.linkCamera(getCamera());
}

AbstractStateMap::~AbstractStateMap() {
}
