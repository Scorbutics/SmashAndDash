#pragma once
#include "AbstractNoGUISceneMap.h"
#include "Physic/System/WorldCollisionResponse.h"
#include "Physic/System/EntityCollisionResponse.h"

class WorldScene;
class FightComponent;

namespace ska {
	class CameraSystem;
	class Window;
	class CollisionSystem;
}

using SceneChangeObserver = ska::Observer<MapEvent>;

class AbstractSceneMap :
	public AbstractNoGUISceneMap, public SceneChangeObserver {
public:
	AbstractSceneMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::SceneHolder& sh, WorldScene& ws, const bool sameMap);
	AbstractSceneMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, Scene& oldScene, WorldScene& ws, const bool sameMap);
	AbstractSceneMap& operator=(const AbstractSceneMap&) = delete;

	bool onTeleport(const MapEvent& me);
	
	virtual ska::CameraSystem& getCamera() = 0;
	virtual ~AbstractSceneMap();

private:
	const bool m_sameMap;
	bool m_observersDefined;

protected:
	virtual void beforeLoad(ska::ScenePtr* lastScene) override;
	virtual void afterLoad(ska::ScenePtr* lastScene) override;
	virtual void onEventUpdate(unsigned int ellapsedTime) override;

	WorldScene& m_worldScene;
	ska::Window& m_window;

	ska::WorldCollisionResponse m_worldCollisionResponse;
	ska::EntityCollisionResponse m_entityCollisionResponse;
	ska::CollisionSystem* m_collisionSystem;
};

