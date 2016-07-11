#pragma once
#include "AbstractNoGUISceneMap.h"
#include "../../ska/Physic/System/CollisionSystem.h"
#include "../../ska/Physic/System/WorldCollisionResponse.h"
#include "../../ska/Physic/System/EntityCollisionResponse.h"

class WorldScene;

namespace ska {
	class CameraSystem;
}

class AbstractSceneMap :
	public AbstractNoGUISceneMap
{
public:
	AbstractSceneMap(WorldScene& ws, ska::SceneHolder& sh, ska::InputContextManager& ril);
	AbstractSceneMap(WorldScene& ws, ska::Scene& oldScene);
	virtual void load(ska::ScenePtr* lastScene) override;
	virtual bool unload() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ska::CameraSystem& getCamera() = 0;
	virtual ~AbstractSceneMap();

protected:
	WorldScene& m_worldScene;
	ska::CollisionSystem m_collisionSystem;

private:
	ska::WorldCollisionResponse m_worldCollisionResponse;
	ska::EntityCollisionResponse m_entityCollisionResponse;
};

