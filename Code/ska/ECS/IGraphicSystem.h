#pragma once
#include "ISystem.h"
namespace ska {
	class DrawableContainer;
	class IGraphicSystem : public ISystem {
	public:
		IGraphicSystem() = default;
		virtual ~IGraphicSystem() = default;

		virtual void setDrawables(DrawableContainer& container) = 0;
	};
}