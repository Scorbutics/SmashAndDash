#pragma once
#include "../../ska/Scene/Scene.h"

class AbstractNoGUISceneMap :
	public ska::Scene
{

public:
	AbstractNoGUISceneMap();
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~AbstractNoGUISceneMap();
};

