#pragma once
#include "HasGraphic.h"
#include "HasLogic.h"

namespace ska {
	class IScene : public HasGraphic, public HasLogic
	{
	public:
		IScene(){}
		virtual ~IScene() {}
	};
}

