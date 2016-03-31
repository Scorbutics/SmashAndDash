#pragma once
#include <memory>
#include "AbstractSceneMap.h"

#include "../../ska/AI/System/IAMovementSystem.h"
#include "../Mobs/System/MobSpawningSystem.h"
#include "../../ska/Script/System/ScriptRefreshSystem.h"
#include "../../Script/System/ScriptCommandsSystem.h"
#include "../Fight/System/FightStartSystem.h"

class WorldScene;

class SceneMap :
	public AbstractSceneMap
{
public:
	SceneMap(ska::SceneHolder& sh, ska::InputContextManager& ril, WorldScene& ws, const std::string fileName, const std::string chipsetName);
	SceneMap(ska::SceneHolder& sh, ska::InputContextManager& ril, WorldScene& ws);
	virtual void load() override;
	virtual void unload() override;
	void reinit();
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~SceneMap();

private:
	
	const std::string m_fileName;
	const std::string m_chipsetName;

	WorldScene& m_worldScene;

	ska::IAMovementSystem m_iaMovementSystem;
	MobSpawningSystem m_mobSpawningSystem;

	ska::ScriptRefreshSystem m_scriptSystem;
	ScriptCommandsSystem m_scriptAutoSystem;

	FightStartSystem m_fightStartSystem;
};
typedef std::unique_ptr<SceneMap> SceneMapPtr;
