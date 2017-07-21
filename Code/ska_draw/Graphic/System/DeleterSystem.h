#pragma once
#include <unordered_set>
#include "ECS/System.h"
#include "ECS/Basics/Graphic/DeleterComponent.h"


namespace ska {
	class DeleterSystem : public System<std::unordered_set<EntityId>, RequiredComponent<DeleterComponent>, PossibleComponent<>> {
	public:
		explicit DeleterSystem(EntityManager& entityManager);
		DeleterSystem& operator=(const DeleterSystem&) = delete;
		virtual ~DeleterSystem();
		virtual void refresh(unsigned int ellapsedTime) override;
	};
}

