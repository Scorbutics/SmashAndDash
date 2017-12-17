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
	AbstractStateMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::World& w);
	AbstractStateMap& operator=(const AbstractStateMap&) = delete;

	bool onTeleport(const MapEvent& me);

	virtual ska::CameraSystem* getCamera() = 0;
	virtual ~AbstractStateMap();

private:

	bool m_observersDefined;
	ska::World& m_world;

protected:
	virtual void beforeLoad(ska::StatePtr* lastScene) override;
	virtual void afterLoad(ska::StatePtr* lastScene) override;

	PokemonGameEventDispatcher& m_eventDispatcher;
	CustomEntityManager& m_entityManager;

	ska::CollisionSystem* m_collisionSystem;
};

