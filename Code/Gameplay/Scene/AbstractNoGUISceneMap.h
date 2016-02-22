#pragma once
#include "IScene.h"
class AbstractNoGUISceneMap :
	public IScene
{

public:
	AbstractNoGUISceneMap();
	virtual void graphicUpdate(void);
	virtual void eventUpdate(bool movingDisallowed);
	virtual ~AbstractNoGUISceneMap();
};

