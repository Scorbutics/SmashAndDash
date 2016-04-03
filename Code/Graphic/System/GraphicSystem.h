#pragma once
#include <unordered_set>
#include "../../ska/Graphic/GraphicComponent.h"
#include "../../ska/Physic/PositionComponent.h"
#include "../../ska/ECS/System.h"
#include "AbstractGraphicSystem.h"

class GraphicSystem : public AbstractGraphicSystem, public ska::System<std::unordered_set<ska::EntityId>, ska::GraphicComponent, ska::PositionComponent>
{
public:
	GraphicSystem(ska::CameraSystem* camera, ska::EntityManager& entityManager);
	virtual ~GraphicSystem();
	virtual void refresh() override;
};


