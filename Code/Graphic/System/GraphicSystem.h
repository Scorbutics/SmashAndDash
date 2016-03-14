#pragma once
#include "../Component/GraphicComponent.h"
#include "../../ska/Physic/PositionComponent.h"
#include "../../ska/ECS/System.h"
#include "../../ska/Graphic/CameraSystem.h"

class GraphicSystem : public ska::System<GraphicComponent, ska::PositionComponent>
{
public:
	GraphicSystem(ska::CameraSystem& camera, ska::EntityManager& entityManager);
	virtual ~GraphicSystem();
protected:
	virtual void refresh(ska::EntityId& entityId) override;
private:
	ska::CameraSystem& m_camera;
};


