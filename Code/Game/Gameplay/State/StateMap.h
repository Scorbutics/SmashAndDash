#pragma once
#include <memory>
#include "StateFight.h"

#ifdef SKA_DEBUG_GRAPHIC
#define AbstractStateMap_ DebugAbstractSceneMap
#include "DebugAbstractSceneMap.h"
#else
#define AbstractSceneMap_ AbstractSceneMap
#include "AbstractStateMap.h"
#endif

class WorldState;

namespace ska {
	class Window;
	class ScriptAutoSystem;
	class ScriptRefreshSystem;
	class CameraSystem;
}

class StateMap :
	public AbstractStateMap_
{
public:
	StateMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::StateHolder& sh, WorldState& ws, const std::string fileName, const std::string chipsetName, const bool sameMap);
	//StateMap(ska::Window& w, ska::StateHolder& sh, PokemonGameEventDispatcher& ged, ska::InputContextManager& ril, WorldState& ws, const bool sameMap);
	StateMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, State& oldScene, WorldState& ws, const std::string fileName, const std::string chipsetName, const bool sameMap);
	virtual void afterLoad(ska::StatePtr* lastScene) override final;
	virtual ska::CameraSystem& getCamera() override;
	virtual ~StateMap();

private:

	const std::string m_fileName;
	const std::string m_chipsetName;
	std::unique_ptr<ska::ScriptAutoSystem> m_scriptAutoSystem;
	ska::CameraSystem* m_cameraSystem;
	ska::ScriptRefreshSystem* m_scriptSystem;

	ska::WorldCollisionResponse m_worldCollisionResponse;
	ska::EntityCollisionResponse m_entityCollisionResponse;
};
typedef std::unique_ptr<StateMap> SceneMapPtr;
