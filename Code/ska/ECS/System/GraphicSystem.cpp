#include "GraphicSystem.h"
#include "../Component/PositionComponent.h"


ska::GraphicSystem::GraphicSystem(EntityManager& entityManager) : System(entityManager) {
}

void ska::GraphicSystem::refresh(EntityId& entityId) {
	PositionComponent compTest;
	compTest.setX(22);
	compTest.setY(-3);
	compTest.setZ(401);
	m_entityManager.addComponent<PositionComponent>(entityId, compTest);
	GraphicComponentPtr& gc = m_entityManager.getComponent<GraphicComponent>(entityId);
	const PositionComponentPtr& pos = m_entityManager.getComponent<PositionComponent>(entityId);
	if (gc != NULL && pos != NULL) {
		gc->getSprite().render(pos->getX(), pos->getY(), NULL);
	}
}

ska::GraphicSystem::~GraphicSystem() {
}
