#pragma once
#include "IScene.h"
class AbstractSceneMap :
	public IScene
{
public:
	AbstractSceneMap();
	virtual void graphicUpdate(void);
	virtual void eventUpdate(bool movingDisallowed);
	virtual ~AbstractSceneMap();
};

