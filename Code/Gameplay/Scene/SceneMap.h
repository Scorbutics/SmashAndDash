#pragma once
#include "AbstractSceneMap.h"
class SceneMap :
	public AbstractSceneMap
{
public:
	SceneMap();
	virtual void graphicUpdate(void) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~SceneMap();
};

