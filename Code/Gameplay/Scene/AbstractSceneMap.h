#pragma once
#include "AbstractNoGUISceneMap.h"
class AbstractSceneMap :
	public AbstractNoGUISceneMap
{
public:
	AbstractSceneMap(ska::EntityManager& em, ska::RawInputListener& ril);
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~AbstractSceneMap();
};

