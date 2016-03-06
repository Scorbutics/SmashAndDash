#include "GraphicSystem.h"

ska::GraphicSystem::GraphicSystem(EntityManager& entityManager) : System(entityManager) {
}

void ska::GraphicSystem::refresh(EntityId& entityId) {
	GraphicComponent& gc = m_entityManager.getComponent<GraphicComponent>(entityId);
	PositionComponent& pos = m_entityManager.getComponent<PositionComponent>(entityId);
	gc.sprite.render(pos.x, pos.y, NULL);
	
}

ska::GraphicSystem::~GraphicSystem() {
}
