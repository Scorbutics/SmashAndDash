#pragma once
#include "AbstractSceneMap.h"
class SceneMap :
	public AbstractSceneMap
{
public:
	SceneMap();
	virtual void graphicUpdate(std::priority_queue<Drawable*>& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~SceneMap();
};

