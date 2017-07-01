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

using StateChangeObserver = ska::Observer<MapEvent>;

class AbstractStateMap :
	public ska::StateBase<CustomEntityManager, PokemonGameEventDispatcher>, 
	public StateChangeObserver {
	
public:
	AbstractStateMap(StateData& data, ska::StateHolder& sh, WorldState& ws, const bool sameMap);
	AbstractStateMap(StateData& data, State& oldScene, WorldState& ws, const bool sameMap);
	AbstractStateMap& operator=(const AbstractStateMap&) = delete;

	bool onTeleport(const MapEvent& me);

	virtual ska::CameraSystem& getCamera() = 0;
	virtual ~AbstractStateMap();

private:

	const bool m_sameMap;
	bool m_observersDefined;

protected:

	virtual void beforeLoad(ska::StatePtr* lastScene) override;
	virtual void afterLoad(ska::StatePtr* lastScene) override;

	PokemonGameEventDispatcher& m_eventDispatcher;
	CustomEntityManager& m_entityManager;
	WorldState& m_worldState;
	ska::Window& m_window;

	ska::CollisionSystem* m_collisionSystem;
};

