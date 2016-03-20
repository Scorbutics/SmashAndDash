#pragma once
#include "CameraSystem.h"
#include "../Draw/DrawableContainer.h"
#include "../PositionnedGraphicDrawable.h"

namespace ska {
	class AbstractGraphicSystem
	{
	public:
		AbstractGraphicSystem(ska::CameraSystem& camera);
		virtual ~AbstractGraphicSystem();
		void setDrawables(DrawableContainer& container);

	protected:
		ska::CameraSystem& m_camera;
		ska::DrawableContainer* m_drawables;
		std::vector<PositionnedGraphicDrawable> m_pgd;
	};
}

