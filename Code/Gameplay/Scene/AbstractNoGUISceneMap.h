#pragma once
#include "IScene.h"


class AbstractNoGUISceneMap :
	public IScene
{

public:
	AbstractNoGUISceneMap();
	virtual void graphicUpdate(DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~AbstractNoGUISceneMap();
};

