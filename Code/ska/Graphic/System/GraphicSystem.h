#pragma once
#include <unordered_set>
#include "../GraphicComponent.h"
#include "../../Physic/PositionComponent.h"
#include "../../ECS/System.h"
#include "AbstractGraphicSystem.h"

namespace ska {
	class GraphicSystem : public AbstractGraphicSystem, public System<std::unordered_set<EntityId>, GraphicComponent, PositionComponent>
	{
	public:
		GraphicSystem(ska::CameraSystem& camera, ska::EntityManager& entityManager);
		virtual ~GraphicSystem();
		virtual void refresh() override;
	};
}

