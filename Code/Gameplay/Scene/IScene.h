#pragma once
#include "HasGraphic.h"
#include "HasLogic.h"

class IScene : public HasGraphic, public HasLogic
{
public:
	IScene(){}
	virtual ~IScene() {}
};

