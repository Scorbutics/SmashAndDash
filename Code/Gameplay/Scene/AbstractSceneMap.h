#pragma once
#include "AbstractNoGUISceneMap.h"
class AbstractSceneMap :
	public AbstractNoGUISceneMap
{
public:
	AbstractSceneMap();
	virtual void graphicUpdate(void);
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~AbstractSceneMap();
};

