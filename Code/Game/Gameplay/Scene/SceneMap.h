#pragma once
#include <memory>

#ifdef SKA_DEBUG_GRAPHIC
#define AbstractSceneMap_ DebugAbstractSceneMap
#include "DebugAbstractSceneMap.h"
#else
#define AbstractSceneMap_ AbstractSceneMap
#include "AbstractSceneMap.h"
#endif

class WorldScene;

namespace ska {
	class Window;
	class ScriptAutoSystem;
	class ScriptRefreshSystem;
	class CameraSystem;
}

class SceneMap :
	public AbstractSceneMap_
{
public:
	SceneMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::SceneHolder& sh, WorldScene& ws, const std::string fileName, const std::string chipsetName, const bool sameMap);
	//SceneMap(ska::Window& w, ska::SceneHolder& sh, PokemonGameEventDispatcher& ged, ska::InputContextManager& ril, WorldScene& ws, const bool sameMap);
	SceneMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, Scene& oldScene, WorldScene& ws, const std::string fileName, const std::string chipsetName, const bool sameMap);
	virtual void afterLoad(ska::ScenePtr* lastScene) override final;
	virtual ska::CameraSystem& getCamera() override;
	virtual ~SceneMap();

private:

	const std::string m_fileName;
	const std::string m_chipsetName;
	std::unique_ptr<ska::ScriptAutoSystem> m_scriptAutoSystem;
	ska::CameraSystem* m_cameraSystem;
	ska::ScriptRefreshSystem* m_scriptSystem;

	ska::WorldCollisionResponse m_worldCollisionResponse;
	ska::EntityCollisionResponse m_entityCollisionResponse;
};
typedef std::unique_ptr<SceneMap> SceneMapPtr;
