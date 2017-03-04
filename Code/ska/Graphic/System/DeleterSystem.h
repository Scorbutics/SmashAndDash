#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../DeleterComponent.h"


namespace ska {
	class DeleterSystem : public System<std::unordered_set<EntityId>, DeleterComponent>
	{
	public:
		DeleterSystem(EntityManager& entityManager);
		virtual ~DeleterSystem();
		virtual void refresh() override;
	};
}

