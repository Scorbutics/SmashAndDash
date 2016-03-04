#include "GraphicSystem.h"
#include "../Component/PositionComponent.h"

ska::GraphicSystem::GraphicSystem()
{
}

void ska::GraphicSystem::refresh(EntityContainer& entities) {
	for (EntityId& id : entities) {
		GraphicComponentPtr& gc = m_components[id];
		const PositionComponent* pos = gc->getPositionComponent();
		if (pos != NULL) {
			gc->getSprite().render(pos->getX(), pos->getY(), NULL);
		}
	}
}

ska::GraphicSystem::~GraphicSystem()
{
}
