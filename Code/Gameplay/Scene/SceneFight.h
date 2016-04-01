#pragma once
#include <memory>
#include "AbstractSceneMap.h"
#include "../../ska/Graphic/System/CameraFixedSystem.h"

class WorldScene;

class SceneFight :
	public AbstractSceneMap
{
public:
	SceneFight(ska::SceneHolder& sh, WorldScene& ws, ska::InputContextManager& ril, ska::Point<int> fightPos);
	virtual void load() override;
	virtual void unload() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~SceneFight();
private:
	WorldScene& m_worldScene;
	ska::CameraFixedSystem m_cameraSystem;
};
typedef std::unique_ptr<SceneFight> SceneFightPtr;
