#pragma once
#include "AbstractNoGUISceneMap.h"
class AbstractSceneMap :
	public AbstractNoGUISceneMap
{
public:
	AbstractSceneMap(ska::SceneHolder& sh, ska::InputContextManager& ril);
	virtual void load() override;
	virtual void unload() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~AbstractSceneMap();
};

