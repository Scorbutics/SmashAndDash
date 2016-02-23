#pragma once
#include "IScene.h"


class AbstractNoGUISceneMap :
	public IScene
{

public:
	AbstractNoGUISceneMap();
	virtual void graphicUpdate(std::priority_queue<Drawable*>& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~AbstractNoGUISceneMap();
};

