#pragma once
#include "AbstractNoGUIStateMap.h"
#include "Physic/System/WorldCollisionResponse.h"

class WorldState;
class FightComponent;

namespace ska {
	class CameraSystem;
	class Window;
	class CollisionSystem;
}

using SceneChangeObserver = ska::Observer<MapEvent>;

class AbstractStateMap :
	public AbstractNoGUIStateMap, public SceneChangeObserver {
public:
	AbstractStateMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::StateHolder& sh, WorldState& ws, const bool sameMap);
	AbstractStateMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, State& oldScene, WorldState& ws, const bool sameMap);
	AbstractStateMap& operator=(const AbstractStateMap&) = delete;

	bool onTeleport(const MapEvent& me);

	virtual ska::CameraSystem& getCamera() = 0;
	virtual ~AbstractStateMap();

private:
	const bool m_sameMap;
	bool m_observersDefined;

protected:
	virtual void beforeLoad(ska::ScenePtr* lastScene) override;
	virtual void afterLoad(ska::ScenePtr* lastScene) override;

	WorldState& m_worldScene;
	ska::Window& m_window;

	ska::CollisionSystem* m_collisionSystem;
};

