#pragma once
#include "ECS/ISystem.h"
#include "Graphic/System/CameraAware.h"

namespace ska {
	class DrawableContainer;
	class CameraSystem;

	class IGraphicSystem : public ISystem, public CameraAware {
	public:
		IGraphicSystem() = default;
		virtual ~IGraphicSystem() = default;

		virtual void setDrawables(DrawableContainer& container) = 0;
	};
}
