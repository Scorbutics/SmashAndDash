#pragma once
#include "../../ska/Scene/Scene.h"

namespace ska {
	class World;
	class EntityManager;
}

class AbstractNoGUISceneMap :
	public ska::Scene
{

public:
	AbstractNoGUISceneMap(ska::SceneHolder& sh, ska::InputContextManager& ril);
	AbstractNoGUISceneMap(ska::Scene& oldScene);
	virtual void load() override;
	virtual void unload() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~AbstractNoGUISceneMap();

};

