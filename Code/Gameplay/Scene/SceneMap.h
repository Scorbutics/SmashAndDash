#pragma once
#include <memory>

#ifdef SKA_DEBUG_GRAPHIC
#define AbstractSceneMap_ DebugAbstractSceneMap
#include "DebugAbstractSceneMap.h"
#else
#define AbstractSceneMap_ AbstractSceneMap
#include "AbstractSceneMap.h"
#endif

#include "../../ska/AI/System/IARandomMovementSystem.h"
#include "../../ska/AI/System/IADefinedMovementSystem.h"
#include "../Mobs/System/MobSpawningSystem.h"
#include "../../ska/Script/System/ScriptRefreshSystem.h"
#include "../../Script/System/ScriptCommandsSystem.h"
#include "../Fight/System/FightStartSystem.h"
#include "../../ska/Graphic/System/CameraFollowSystem.h"


class WorldScene;

namespace ska {
	class Window;
}

class SceneMap :
	public AbstractSceneMap_
{
public:
	SceneMap(ska::Window& w, ska::SceneHolder& sh, ska::InputContextManager& ril, WorldScene& ws, const std::string fileName, const std::string chipsetName, const bool sameMap);
	SceneMap(ska::Window& w, ska::SceneHolder& sh, ska::InputContextManager& ril, WorldScene& ws, const bool sameMap);
	SceneMap(ska::Window& w, ska::Scene& oldScene, WorldScene& ws, const std::string fileName, const std::string chipsetName, const bool sameMap);
	virtual void load(ska::ScenePtr* lastScene) override;
	virtual bool unload() override;
	void reinit();
	virtual ska::CameraSystem& getCamera() override;
	virtual ~SceneMap();

private:

	const std::string m_fileName;
	const std::string m_chipsetName;

	ska::IARandomMovementSystem m_iaRandomMovementSystem;
	MobSpawningSystem m_mobSpawningSystem;

	ska::ScriptRefreshSystem m_scriptSystem;
	ScriptCommandsSystem m_scriptAutoSystem;
	ska::IADefinedMovementSystem m_iaDefinedMovementSystem;

	FightStartSystem m_fightStartSystem;
	ska::CameraFollowSystem m_cameraSystem;
};
typedef std::unique_ptr<SceneMap> SceneMapPtr;
