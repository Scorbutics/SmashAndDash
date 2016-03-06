#pragma once
#include "../../ska/Scene/Scene.h"

class AbstractNoGUISceneMap :
	public ska::Scene
{

public:
	AbstractNoGUISceneMap();
	virtual void graphicUpdate(DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~AbstractNoGUISceneMap();
};

