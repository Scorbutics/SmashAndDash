#include <limits>
#include "CameraSystem.h"
#include "Draw/DrawableContainer.h"
#include "GraphicSystem.h"
#include "../PositionnedGraphicDrawable.h"
#include "ECS/Basics/Graphic/DialogComponent.h"
#include "Utils/NumberUtils.h"

ska::GraphicSystem::GraphicSystem(ska::EntityManager& entityManager, ska::CameraSystem* camera) :
    AbstractGraphicSystem(camera),
    System(entityManager) {
	m_drawables = nullptr;
}

void ska::GraphicSystem::refresh() {
	const ska::Rectangle* camera = m_camera == nullptr ? nullptr : m_camera->getDisplay();
	const unsigned int cameraX = (camera == nullptr || camera->x < 0 ? 0 : camera->x);
	const unsigned int cameraY = (camera == nullptr || camera->y < 0 ? 0 : camera->y);

	if (m_drawables == nullptr) {
		return;
	}

	m_topLayerPriority = std::numeric_limits<int>::min();
	m_pgd.clear();

	for (const ska::EntityId entityId : m_processed) {
		ska::GraphicComponent& gc = m_entityManager.getComponent<ska::GraphicComponent>(entityId);
		ska::PositionComponent& pos = m_entityManager.getComponent<ska::PositionComponent>(entityId);
		const int relPosX = pos.x - cameraX;
		const int relPosY = pos.y - cameraY - pos.z;

		for (auto& sprite : gc.sprite) {
			if (!((relPosX + sprite.getWidth()) < 0 || (camera != nullptr && relPosX >= camera->w) ||
				(relPosY + sprite.getHeight()) < 0 || (camera != nullptr && relPosY >= camera->h))) {
				m_topLayerPriority = ska::NumberUtils::maximum(pos.y, m_topLayerPriority);
				const int priority = gc.desiredPriority > std::numeric_limits<int>::min() ? gc.desiredPriority : pos.y + (camera == nullptr ? 0 : camera->h*pos.z);
				m_pgd.push_back(ska::PositionnedGraphicDrawable(sprite, relPosX, relPosY, priority, m_topLayerPriority));
			}
		}

		if (m_entityManager.hasComponent<DialogComponent>(entityId)) {
			//DialogComponent& dc = m_entityManager.getComponent<DialogComponent>(entityId);
			/*dc.dialog.move({ static_cast<int>(pos.x - cameraX), static_cast<int>(pos.y - cameraY - dc.dialog.getBox().h) });
			dc.dialog.refresh();
			if (dc.dialog.isVisible()) {
				m_drawables->add(dc.dialog);
			} else {*/
				m_entityManager.removeComponent<DialogComponent>(entityId);
			//}
		}
	}
	for (auto& pgd : m_pgd) {
		m_drawables->add(pgd);
	}

	m_drawables = nullptr;
}

void ska::GraphicSystem::update() {
	System::update();
}

int ska::GraphicSystem::getTopLayerPriority() const {
	return m_topLayerPriority + 1;
}

ska::GraphicSystem::~GraphicSystem() {
}
