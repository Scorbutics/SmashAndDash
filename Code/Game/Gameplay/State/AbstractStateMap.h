#pragma once
#include "Core/State/StateBase.h"
#include "Physic/System/WorldCollisionResponse.h"
#include "../CustomEntityManager.h"

class WorldState;
class FightComponent;

namespace ska {
	class CameraSystem;
	class Window;
	class CollisionSystem;
}

class AbstractStateMap :
	public ska::StateBase {
	
public:
	AbstractStateMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& ws);
	AbstractStateMap& operator=(const AbstractStateMap&) = delete;

	virtual ska::CameraSystem* getCamera() = 0;
	virtual ~AbstractStateMap();

private:

	bool m_observersDefined;
	WorldState& m_worldState;

protected:
	virtual void beforeLoad(ska::State* lastScene) override;
	virtual void afterLoad(ska::State* lastScene) override;

	PokemonGameEventDispatcher& m_eventDispatcher;
	CustomEntityManager& m_entityManager;

	ska::CollisionSystem* m_collisionSystem;
};

