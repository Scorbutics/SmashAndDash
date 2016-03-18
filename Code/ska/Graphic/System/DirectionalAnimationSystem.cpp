#include "DirectionalAnimationSystem.h"
#include "../../Utils/RectangleUtils.h"

ska::DirectionalAnimationSystem::DirectionalAnimationSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void ska::DirectionalAnimationSystem::refresh() {
	for (EntityId entityId : m_processed) {
		GraphicComponent& gc = m_entityManager.getComponent<GraphicComponent>(entityId);
		DirectionalAnimationComponent& dac = m_entityManager.getComponent<DirectionalAnimationComponent>(entityId);
		MovementComponent& mov = m_entityManager.getComponent<MovementComponent>(entityId);

		//ska::Rectangle base de l'animation
		Rectangle spritePos = gc.sprite.getOffsetBase();
		const int spriteHeight = gc.sprite.getHeight();
		

		if (((int)mov.vx) == 0 && ((int)mov.vy) == 0) {
			gc.sprite.stop(true);
			gc.sprite.reset();
		} else {
			gc.sprite.stop(false);
		}

		dac.direction = RectangleUtils::getDirectionFromPos(Point<int>( 0, 0 ), Point<int>( (int)mov.vx, (int)mov.vy ));

		switch (dac.direction)
		{
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

		gc.sprite.setOffset(spritePos);
	}
}

ska::DirectionalAnimationSystem::~DirectionalAnimationSystem() {
}
