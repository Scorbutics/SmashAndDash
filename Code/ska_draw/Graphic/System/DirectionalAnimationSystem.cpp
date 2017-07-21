#include "DirectionalAnimationSystem.h"
#include "Utils/RectangleUtils.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "Logging/Logger.h"

ska::DirectionalAnimationSystem::DirectionalAnimationSystem(EntityManager& entityManager) : System(entityManager) {
}

void ska::DirectionalAnimationSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	for (auto entityId : processed) {
		auto& gc = m_componentAccessor.get<GraphicComponent>(entityId);
		auto& dac = m_componentAccessor.get<DirectionalAnimationComponent>(entityId);
		auto& mov = m_componentAccessor.get<MovementComponent>(entityId);

		if (gc.sprite.empty()) {
			break;
		}

		//ska::Rectangle base de l'animation
		auto& texture = gc.sprite[0];
		auto spritePos = texture.getOffsetBase();
		const int spriteHeight = texture.getHeight();

		if (static_cast<int>(mov.vx) == 0 &&
			static_cast<int>(mov.vy) == 0 ) {
			texture.stop(true);
			texture.reset();
		} else {
			texture.stop(false);
		}

		auto xMove = ska::NumberUtils::round(mov.vx);
		auto yMove = ska::NumberUtils::round(mov.vy);

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

			const auto& pcPtr = m_componentPossibleAccessor.get<PositionComponent>(dac.looked);
			if (pcPtr != nullptr) {
				auto& pcLooked = *pcPtr;
				dac.direction = RectangleUtils::getDirectionFromPos(*pcPtr, pcLooked);
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
