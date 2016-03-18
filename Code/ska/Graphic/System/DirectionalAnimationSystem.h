#pragma once
#include <unordered_set>
#include "../GraphicComponent.h"
#include "../DirectionalAnimationComponent.h"
#include "../../Physic/MovementComponent.h"
//#include "../../Physic/PositionComponent.h"
#include "../../ECS/System.h"
#include "CameraSystem.h"
#include "../Draw/DrawableContainer.h"
#include "../PositionnedGraphicDrawable.h"

namespace ska {
	class DirectionalAnimationSystem : public ska::System<std::unordered_set<ska::EntityId>, GraphicComponent, MovementComponent, DirectionalAnimationComponent>
	{
	public:
		DirectionalAnimationSystem(ska::EntityManager& entityManager);
		virtual ~DirectionalAnimationSystem();
		virtual void refresh() override;

	};
}

