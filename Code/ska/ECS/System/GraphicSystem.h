#pragma once
#include "../Component/GraphicComponent.h"
#include "System.h"

namespace ska {
	class GraphicSystem : public System
	{
	public:
		GraphicSystem(EntityManager& entityManager);
		virtual ~GraphicSystem();
	protected:
		virtual void refresh(EntityId& EntityId) override;
	};
}

