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

struct WorldStateChanger;
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
	StateMap(StateData& data, ska::StateHolder& sh, WorldStateChanger& wsc);
	StateMap(StateData& data, State& oldScene, WorldStateChanger& wsc);
	virtual void afterLoad(ska::StatePtr* lastScene) override final;
	virtual ska::CameraSystem& getCamera() override;
	virtual ~StateMap();

private:
	void init(StateData& data, WorldState& ws);

	const std::string m_fileName;
	const std::string m_chipsetName;
	std::unique_ptr<ska::ScriptAutoSystem> m_scriptAutoSystem;
	ska::CameraSystem* m_cameraSystem;
	ska::ScriptRefreshSystem* m_scriptSystem;

	ska::WorldCollisionResponse m_worldCollisionResponse;
	ska::EntityCollisionResponse m_entityCollisionResponse;
};
typedef std::unique_ptr<StateMap> SceneMapPtr;
