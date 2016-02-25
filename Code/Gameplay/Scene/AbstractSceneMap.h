#pragma once
#include "AbstractNoGUISceneMap.h"
class AbstractSceneMap :
	public AbstractNoGUISceneMap
{
public:
	AbstractSceneMap();
	virtual void graphicUpdate(DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~AbstractSceneMap();
};

