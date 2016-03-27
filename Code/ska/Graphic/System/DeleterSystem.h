#pragma once
#include <unordered_set>
#include "../../ECS/System.h"
#include "../DeleterComponent.h"


namespace ska {
	class DeleterSystem : public ska::System<std::unordered_set<ska::EntityId>, DeleterComponent>
	{
	public:
		DeleterSystem(ska::EntityManager& entityManager);
		virtual ~DeleterSystem();
		virtual void refresh() override;
	};
}

