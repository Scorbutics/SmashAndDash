#pragma once
#include "Core/Scene/Scene.h"

namespace ska {
	class World;
	class EntityManager;
}

class AbstractNoGUISceneMap :
	public ska::Scene {

public:
	AbstractNoGUISceneMap(ska::SceneHolder& sh, ska::InputContextManager& ril);
	explicit AbstractNoGUISceneMap(Scene& oldScene);
	AbstractNoGUISceneMap& operator=(const AbstractNoGUISceneMap&) = delete;

	virtual void load(ska::ScenePtr* lastScene) override;
	virtual bool unload() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(unsigned int ellapsedTime) override;
	virtual ~AbstractNoGUISceneMap();

};

