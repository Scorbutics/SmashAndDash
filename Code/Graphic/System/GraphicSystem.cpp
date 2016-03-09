#include "GraphicSystem.h"

GraphicSystem::GraphicSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void GraphicSystem::refresh(ska::EntityId& entityId) {
	GraphicComponent& gc = m_entityManager.getComponent<GraphicComponent>(entityId);
	PositionComponent& pos = m_entityManager.getComponent<PositionComponent>(entityId);
	gc.sprite.render(pos.x, pos.y, NULL);
	
}

GraphicSystem::~GraphicSystem() {
}
