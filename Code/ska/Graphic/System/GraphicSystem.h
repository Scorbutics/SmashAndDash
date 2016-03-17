#pragma once
#include <unordered_set>
#include "../GraphicComponent.h"
#include "../../Physic/PositionComponent.h"
#include "../../ECS/System.h"
#include "CameraSystem.h"
#include "../Draw/VectorDrawableContainer.h"
#include "../PositionnedGraphicDrawable.h"

namespace ska {
	class GraphicSystem : public ska::System<std::unordered_set<ska::EntityId>, GraphicComponent, ska::PositionComponent>
	{
	public:
		GraphicSystem(ska::CameraSystem& camera, ska::EntityManager& entityManager);
		virtual ~GraphicSystem();
		virtual void refresh() override;
		ska::VectorDrawableContainer& getDrawables();

	private:
		ska::CameraSystem& m_camera;
		ska::VectorDrawableContainer m_drawables;
		std::vector<PositionnedGraphicDrawable> m_pgd;
	};
}

