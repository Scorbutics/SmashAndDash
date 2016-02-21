#pragma once
#include "AbstractSceneMap.h"
class SceneFight :
	public AbstractSceneMap
{
public:
	SceneFight();
	virtual void graphicUpdate(void);
	virtual void eventUpdate(bool movingDisallowed);
	virtual ~SceneFight();
};

