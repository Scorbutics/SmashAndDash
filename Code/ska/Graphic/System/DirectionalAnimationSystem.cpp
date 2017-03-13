#include "DirectionalAnimationSystem.h"
#include "../../Utils/RectangleUtils.h"
#include "../../Physic/PositionComponent.h"

ska::DirectionalAnimationSystem::DirectionalAnimationSystem(EntityManager& entityManager) : System(entityManager) {
}

void ska::DirectionalAnimationSystem::refresh() {
	for (auto entityId : m_processed) {
		auto& gc = m_entityManager.getComponent<GraphicComponent>(entityId);
		auto& dac = m_entityManager.getComponent<DirectionalAnimationComponent>(entityId);
		auto& mov = m_entityManager.getComponent<MovementComponent>(entityId);

		if (gc.sprite.empty()) {
			break;
		}

		//ska::Rectangle base de l'animation
		auto& texture = gc.sprite[0];
		auto spritePos = texture.getOffsetBase();
		const int spriteHeight = texture.getHeight();
		

		if (static_cast<int>(mov.vx) == 0 && static_cast<int>(mov.vy) == 0) {
			texture.stop(true);
			texture.reset();
		} else {
			texture.stop(false);
		}
		
		const auto xMove = static_cast<int>(mov.vx + mov.ax + 0.5);
		const auto yMove = static_cast<int>(mov.vy + mov.ay + 0.5);


		//If it begins to be difficult to maintain, create two components with two different systems to handle each movement type
		switch(dac.type) {
		case DirectionalAnimationType::MOVEMENT_THEN_FOLLOWING:
		case DirectionalAnimationType::MOVEMENT:  {
			if (xMove != 0 || yMove != 0) {
				dac.direction = RectangleUtils::getDirectionFromPos(Point<int>(0, 0), Point<int>(xMove, yMove));
				break;
			}
			
			if (dac.type == DirectionalAnimationType::MOVEMENT) {
				break;
			}

			if (m_entityManager.hasComponent<PositionComponent>(dac.looked)) {
				auto& pc = m_entityManager.getComponent<PositionComponent>(entityId);
				auto& pcLooked = m_entityManager.getComponent<PositionComponent>(dac.looked);
				dac.direction = RectangleUtils::getDirectionFromPos(pc, pcLooked);
			}
			break;
		}
		 
		default: break;
		}

		switch (dac.direction) {
		case 0:
			spritePos.y = spriteHeight * 2;
			break;
		case 1:
			spritePos.y = spriteHeight;
			break;
		case 2:
			spritePos.y = 0;
			break;
		case 3:
			spritePos.y = spriteHeight * 3;
			break;
		case 4:
			spritePos.y = spriteHeight * 7;
			break;
		case 5:
			spritePos.y = spriteHeight * 5;
			break;
		case 6:
			spritePos.y = spriteHeight * 4;
			break;
		case 7:
			spritePos.y = spriteHeight * 6;
			break;
		default:
			break;
		}


		texture.setOffset(spritePos);
	}
}

ska::DirectionalAnimationSystem::~DirectionalAnimationSystem() {
}
