#pragma once
#include "AbstractSceneMap.h"
class SceneFight :
	public AbstractSceneMap
{
public:
	SceneFight(ska::EntityManager& em, ska::RawInputListener& ril);
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~SceneFight();
};

