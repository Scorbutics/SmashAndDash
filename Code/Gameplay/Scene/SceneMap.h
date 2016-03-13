#pragma once
#include "AbstractSceneMap.h"
class SceneMap :
	public AbstractSceneMap
{
public:
	SceneMap(ska::RawInputListener& ril);
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~SceneMap();
};

