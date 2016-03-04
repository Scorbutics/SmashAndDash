#pragma once
#include "../Component/GraphicComponent.h"
#include "System.h"

namespace ska {
	class GraphicSystem : public System<GraphicComponent>
	{
	public:
		GraphicSystem();
		virtual void refresh(EntityContainer& entities) override;
		virtual ~GraphicSystem();
	};
}

