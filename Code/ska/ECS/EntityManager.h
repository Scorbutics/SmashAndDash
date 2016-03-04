#pragma once
#include "ECSDefines.h"
#include "Component/Component.h"

namespace ska {
	class EntityManager
	{
	public:
		EntityManager();
		EntityId createEntity(ComponentPtr& comp...);

		~EntityManager();
	};
}
