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
	virtual void load(ska::ScenePtr* lastScene) override;
	virtual bool unload() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(unsigned int ellapsedTime) override;
	virtual ska::CameraSystem& getCamera() = 0;
	virtual ~AbstractSceneMap();

private:
	const bool m_sameMap;
	bool m_observersDefined;

protected:
	WorldScene& m_worldScene;
	ska::Window& m_window;

	ska::WorldCollisionResponse m_worldCollisionResponse;
	ska::EntityCollisionResponse m_entityCollisionResponse;
	ska::CollisionSystem* m_collisionSystem;
};

