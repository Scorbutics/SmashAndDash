#pragma once
#include <unordered_set>
#include "../../Physic/PositionComponent.h"
#include "../../ECS/System.h"
#include "../HasShadowComponent.h"
#include "../../Physic/HitboxComponent.h"
#include "AbstractGraphicSystem.h"

namespace ska {
	class ShadowSystem : public AbstractGraphicSystem, public ska::System<std::unordered_set<ska::EntityId>, PositionComponent, HitboxComponent, HasShadowComponent>
	{
	public:
		ShadowSystem(ska::CameraSystem* camera, ska::EntityManager& entityManager);
		virtual ~ShadowSystem();
		virtual void refresh() override;
	
	private:
		AnimatedTexture m_shadow;
	};
}

