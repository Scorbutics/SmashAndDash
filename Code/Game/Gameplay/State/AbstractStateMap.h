#pragma once
#include "Core/State/StateBase.h"
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

	~AbstractStateMap() override = default;

private:

	bool m_observersDefined;
	WorldState& m_worldState;

protected:
	virtual void beforeLoad(ska::State* lastState) override;
	virtual void afterLoad(ska::State* lastState) override;

	PokemonGameEventDispatcher& m_eventDispatcher;
	CustomEntityManager& m_entityManager;

	ska::CollisionSystem* m_collisionSystem;
};

