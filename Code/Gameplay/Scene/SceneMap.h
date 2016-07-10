#pragma once
#include <memory>
#include "AbstractSceneMap.h"

#include "../../ska/AI/System/IARandomMovementSystem.h"
#include "../../ska/AI/System/IADefinedMovementSystem.h"
#include "../Mobs/System/MobSpawningSystem.h"
#include "../../ska/Script/System/ScriptRefreshSystem.h"
#include "../../Script/System/ScriptCommandsSystem.h"
#include "../Fight/System/FightStartSystem.h"
#include "../../ska/Graphic/System/CameraFollowSystem.h"
#include "../../ska/Physic/System/CollisionSystem.h"
#include "../../ska/Physic/System/WorldCollisionResponse.h"
#include "../../ska/Physic/System/EntityCollisionResponse.h"

class WorldScene;

class SceneMap :
	public AbstractSceneMap
{
public:
	SceneMap(ska::SceneHolder& sh, ska::InputContextManager& ril, WorldScene& ws, const std::string fileName, const std::string chipsetName);
	SceneMap(ska::SceneHolder& sh, ska::InputContextManager& ril, WorldScene& ws);
	SceneMap(ska::Scene& oldScene, WorldScene& ws, const std::string fileName, const std::string chipsetName);
	virtual void load(ska::ScenePtr* lastScene) override;
	virtual bool unload() override;
	void reinit();
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~SceneMap();

private:
	
	const std::string m_fileName;
	const std::string m_chipsetName;

	WorldScene& m_worldScene;

	ska::IARandomMovementSystem m_iaRandomMovementSystem;
	MobSpawningSystem m_mobSpawningSystem;

	ska::ScriptRefreshSystem m_scriptSystem;
	ScriptCommandsSystem m_scriptAutoSystem;
	ska::IADefinedMovementSystem m_iaDefinedMovementSystem;

	FightStartSystem m_fightStartSystem;
	ska::CameraFollowSystem m_cameraSystem;
	ska::CollisionSystem m_collisionSystem;

	ska::WorldCollisionResponse m_worldCollisionResponse;
	ska::EntityCollisionResponse m_entityCollisionResponse;
};
typedef std::unique_ptr<SceneMap> SceneMapPtr;
