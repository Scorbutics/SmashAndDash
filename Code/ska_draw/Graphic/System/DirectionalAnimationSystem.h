#pragma once
#include <unordered_set>
#include "../GraphicComponent.h"
#include "ECS/Basics/Graphic/DirectionalAnimationComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/System.h"
#include "ECS/Basics/Physic/PositionComponent.h"

namespace ska {
	class DirectionalAnimationSystem : public System<std::unordered_set<EntityId>, RequiredComponent<GraphicComponent, MovementComponent, DirectionalAnimationComponent>, PossibleComponent<PositionComponent>> {
	public:
		explicit DirectionalAnimationSystem(EntityManager& entityManager);
		DirectionalAnimationSystem& operator=(const DirectionalAnimationSystem&) = delete;
		virtual ~DirectionalAnimationSystem();
		virtual void refresh(unsigned int ellapsedTime) override;

	};
}

