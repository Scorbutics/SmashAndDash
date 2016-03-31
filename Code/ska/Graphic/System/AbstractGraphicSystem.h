#pragma once
#include "../../ECS/IGraphicSystem.h"
#include "../PositionnedGraphicDrawable.h"

namespace ska {
	class CameraSystem;

	class AbstractGraphicSystem : public IGraphicSystem
	{
	public:
		AbstractGraphicSystem(ska::CameraSystem& camera);
		virtual ~AbstractGraphicSystem();
		virtual void setDrawables(DrawableContainer& container) override;

	protected:
		ska::CameraSystem& m_camera;
		ska::DrawableContainer* m_drawables;
		std::vector<PositionnedGraphicDrawable> m_pgd;
	};
}

