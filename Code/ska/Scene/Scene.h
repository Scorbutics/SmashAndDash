#pragma once
#include "HasGraphic.h"
#include "HasLogic.h"

namespace ska {
	class Scene : public HasGraphic, public HasLogic
	{
	public:
		Scene(){}
		virtual ~Scene() {}
	};
}

