#pragma once
#include "AbstractNoGUISceneMap.h"
#include "../../ska/Physic/System/CollisionSystem.h"
#include "../../ska/Physic/System/WorldCollisionResponse.h"
#include "../../ska/Physic/System/EntityCollisionResponse.h"
#include "../../ska/Graphic/Point.h"
#include "../../ska/Core/GameEventDispatcher.h"

class WorldScene;
class FightComponent;

namespace ska {
	class CameraSystem;
	class Window;

	template <typename T>
	class SceneSwitcher;
}

using SceneChangeObserver = ska::Observer<ska::SceneSwitcher<WorldScene&>>;

class AbstractSceneMap :
	public AbstractNoGUISceneMap, public SceneChangeObserver {
public:
	AbstractSceneMap(ska::Window& w, WorldScene& ws, ska::GameEventDispatcher& ged, ska::SceneHolder& sh, ska::InputContextManager& ril, const bool sameMap);
	AbstractSceneMap(ska::Window& w, WorldScene& ws, ska::GameEventDispatcher& ged, Scene& oldScene, const bool sameMap);
	AbstractSceneMap& operator=(const AbstractSceneMap&) = delete;

	bool onTeleport(const ska::SceneSwitcher<WorldScene&>& switcher);
	virtual void load(ska::ScenePtr* lastScene) override;
	virtual bool unload() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ska::CameraSystem& getCamera() = 0;
	virtual ~AbstractSceneMap();

private: 
	const bool m_sameMap;

protected:

	WorldScene& m_worldScene;
	ska::Window& m_window;
	ska::CollisionSystem m_collisionSystem;

	ska::WorldCollisionResponse m_worldCollisionResponse;
	ska::EntityCollisionResponse m_entityCollisionResponse;
};

