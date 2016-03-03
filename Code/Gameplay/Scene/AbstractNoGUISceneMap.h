#pragma once
#include "../../ska/Scene/IScene.h"

class AbstractNoGUISceneMap :
	public ska::IScene
{

public:
	AbstractNoGUISceneMap();
	virtual void graphicUpdate(DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~AbstractNoGUISceneMap();
};

