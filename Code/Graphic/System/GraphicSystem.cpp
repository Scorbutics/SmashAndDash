#include "GraphicSystem.h"

GraphicSystem::GraphicSystem(ska::CameraSystem& camera, ska::EntityManager& entityManager) : System(entityManager), m_camera(camera) {
}

void GraphicSystem::refresh(ska::EntityId& entityId) {
	GraphicComponent& gc = m_entityManager.getComponent<GraphicComponent>(entityId);
	ska::PositionComponent& pos = m_entityManager.getComponent<ska::PositionComponent>(entityId);
	const ska::Rectangle* camera = m_camera.getDisplay();
	gc.sprite.render(pos.x + (camera == NULL ? 0 : camera->x), pos.y + (camera == NULL ? 0 : camera->y), NULL);
	
}

GraphicSystem::~GraphicSystem() {
}
