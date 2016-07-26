#include "../../ska/Graphic/System/CameraSystem.h"
#include "../../ska/Graphic/Draw/DrawableContainer.h"
#include "GraphicSystem.h"
#include "../../ska/Graphic/PositionnedGraphicDrawable.h"
#include "../DialogComponent.h"
#include "../GUI/Window_Area.h"
#include "../../ska/Utils/NumberUtils.h"

typedef std::unique_ptr<DialogMenu> DialogMenuPtr;

GraphicSystem::GraphicSystem(ska::CameraSystem* camera, ska::EntityManager& entityManager) : System(entityManager), AbstractGraphicSystem(camera) {
	m_drawables = NULL;
}

void GraphicSystem::refresh() {
	const ska::Rectangle* camera = m_camera == NULL ? NULL : m_camera->getDisplay();
	const unsigned int cameraX = (camera == NULL || camera->x < 0 ? 0 : camera->x);
	const unsigned int cameraY = (camera == NULL || camera->y < 0 ? 0 : camera->y);

	if (m_drawables == NULL) {
		return;
	}
	
	m_topLayerPriority = INT_MIN;
	m_pgd.clear();
	
	for (ska::EntityId entityId : m_processed) {
		ska::GraphicComponent& gc = m_entityManager.getComponent<ska::GraphicComponent>(entityId);
		ska::PositionComponent& pos = m_entityManager.getComponent<ska::PositionComponent>(entityId);
		const int relPosX = pos.x - cameraX;
		const int relPosY = pos.y - cameraY - pos.z;

		for (auto& sprite : gc.sprite) {
			if (!(relPosX + sprite.getWidth() < 0 || camera != NULL && relPosX >= camera->w ||
				relPosY + sprite.getHeight() < 0 || camera != NULL && relPosY >= camera->h)) {
				m_topLayerPriority = ska::NumberUtils::maximum(pos.y, m_topLayerPriority);
				const int priority = gc.desiredPriority > INT_MIN ? gc.desiredPriority : pos.y + (camera == NULL ? 0 : camera->h*pos.z);
				m_pgd.push_back(ska::PositionnedGraphicDrawable(sprite, relPosX, relPosY, priority, m_topLayerPriority));
			}
		}

		if (m_entityManager.hasComponent<DialogComponent>(entityId)) {
			DialogComponent& dc = m_entityManager.getComponent<DialogComponent>(entityId);
			dc.dialog.setPos({ static_cast<int>(pos.x - cameraX), static_cast<int>(pos.y - cameraY - dc.dialog.getRect().h) });
			dc.dialog.refresh();
			if (dc.dialog.isVisible()) {	
				m_drawables->add(dc.dialog);
			} else {
				m_entityManager.removeComponent<DialogComponent>(entityId);
			}
		}
	}
	for (auto& pgd : m_pgd) {
		m_drawables->add(pgd);
	}

	m_drawables = NULL;
}

void GraphicSystem::update() {
	System::update();
}

int GraphicSystem::getTopLayerPriority() const {
	return m_topLayerPriority + 1;
}

GraphicSystem::~GraphicSystem() {
}
