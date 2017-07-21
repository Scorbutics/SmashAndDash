#include "DebugCollisionDrawerSystem.h"
#include "Utils/SpritePath.h"
#include "Utils/SkaConstants.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "../GraphicComponent.h"
#include "ECS/Basics/Graphic/DeleterComponent.h"

ska::DebugCollisionDrawerSystem::DebugCollisionDrawerSystem(EntityManager& em) : System(em) {
}

void ska::DebugCollisionDrawerSystem::refresh(unsigned int) {
	std::vector<EntityId> removeComponents;

	const auto& processed = getEntities();
	for (auto entityId : processed) {
		auto& dcgc = m_componentAccessor.get<DebugGraphicComponent>(entityId);

		auto* wcolPtr = m_componentPossibleAccessor.get<WorldCollisionComponent>(entityId);
		if (((dcgc.typeMask & COLLISION) == COLLISION) && wcolPtr != nullptr) {
			auto& wcol = *wcolPtr;

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
			createDebugCollisionEntity(PositionComponent::getCenterPosition(m_componentAccessor.get<PositionComponent>(entityId), m_componentAccessor.get<HitboxComponent>(entityId)), 1);
		}
		removeComponents.push_back(entityId);
	}

	for (auto& e : removeComponents) {
		m_componentAccessor.remove<DebugGraphicComponent>(e);
	}
}

ska::EntityId ska::DebugCollisionDrawerSystem::createDebugCollisionEntity(const Point<int>& p, int offset) {
	EntityId e = createEntity();
	GraphicComponent gc;
	gc.sprite.resize(1);
	gc.sprite[0].load(SpritePath::getInstance().getPath(SPRITEBANK_ANIMATION, 18 + offset), 2, 1, 2, false, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, offset == 0 ? 40 : 100);
	gc.sprite[0].setDelay(500);
	m_componentAccessor.add<GraphicComponent>(e, gc);
	DeleterComponent dc;
	dc.delay = 3000;
	m_componentAccessor.add<DeleterComponent>(e, dc);
	PositionComponent pc = p;
	m_componentAccessor.add<PositionComponent>(e, pc);
	return e;
}
