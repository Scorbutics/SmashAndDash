#pragma once
#include "AbstractSceneMap.h"
class SceneFight :
	public AbstractSceneMap
{
public:
	SceneFight();
	virtual void graphicUpdate(DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~SceneFight();
};

