#pragma once
#include "../Component/GraphicComponent.h"
#include "../Component/PositionComponent.h"
#include "../Core/System/System.h"

namespace ska {
	class GraphicSystem : public System<GraphicComponent, PositionComponent>
	{
	public:
		GraphicSystem(EntityManager& entityManager);
		virtual ~GraphicSystem();
	protected:
		virtual void refresh(EntityId& EntityId) override;
	};
}

