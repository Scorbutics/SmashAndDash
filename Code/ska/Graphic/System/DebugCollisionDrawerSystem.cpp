#include "DebugCollisionDrawerSystem.h"
#include "../SpritePath.h"
#include "../../Utils/SkaConstants.h"
#include "../../Physic/PositionComponent.h"
#include "../GraphicComponent.h"
#include "../DeleterComponent.h"

ska::DebugCollisionDrawerSystem::DebugCollisionDrawerSystem(EntityManager& em) : System(em) {
}

void ska::DebugCollisionDrawerSystem::refresh() {
	std::vector<EntityId> removeComponents;
	for (EntityId entityId : m_processed) {

		DebugGraphicComponent& dcgc = m_entityManager.getComponent<DebugGraphicComponent>(entityId);
		if (((dcgc.typeMask & COLLISION) == COLLISION) && m_entityManager.hasComponent<WorldCollisionComponent>(entityId)) {
			WorldCollisionComponent& wcol = m_entityManager.getComponent<WorldCollisionComponent>(entityId);

			if (dcgc.collidedBlocks.size() >= 10) {
				scheduleDeferredRemove(dcgc.collidedBlocks.front());
				dcgc.collidedBlocks.pop();
				scheduleDeferredRemove(dcgc.collidedBlocks.front());
				dcgc.collidedBlocks.pop();
			}

			if (wcol.xaxis) {
				for (const auto& p : wcol.blockColPosX) {
					dcgc.collidedBlocks.push(createDebugCollisionEntity(p, 0));
				}
			}

			if (wcol.yaxis) {
				for (const auto& p : wcol.blockColPosY) {
					dcgc.collidedBlocks.push(createDebugCollisionEntity(p, 0));
				}
			}
		}
		if ((dcgc.typeMask & WALK) == WALK) {
			createDebugCollisionEntity(PositionComponent::getCenterPosition(m_entityManager.getComponent<PositionComponent>(entityId), m_entityManager.getComponent<HitboxComponent>(entityId)), 1);
		}
		removeComponents.push_back(entityId);
	}

	for (auto& e : removeComponents) {
		m_entityManager.removeComponent<DebugGraphicComponent>(e);
	}
}

ska::EntityId ska::DebugCollisionDrawerSystem::createDebugCollisionEntity(const Point<int>& p, int offset) const {
	EntityId e = m_entityManager.createEntity();
	GraphicComponent gc;
	gc.sprite.resize(1);
	gc.sprite[0].load(SpritePath::getInstance().getPath(SPRITEBANK_ANIMATION, 18 + offset), 2, 1, 2, false, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, offset == 0 ? 40 : 100);
	gc.sprite[0].setDelay(500);
	m_entityManager.addComponent<GraphicComponent>(e, gc);
	DeleterComponent dc;
	dc.delay = 3000;
	m_entityManager.addComponent<DeleterComponent>(e, dc);
	PositionComponent pc = p;
	m_entityManager.addComponent<PositionComponent>(e, pc);
	return e;
}