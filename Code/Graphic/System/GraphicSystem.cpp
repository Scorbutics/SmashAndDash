#include "../../ska/Graphic/System/CameraSystem.h"
#include "../../ska/Graphic/Draw/DrawableContainer.h"
#include "GraphicSystem.h"
#include "../../ska/Graphic/PositionnedGraphicDrawable.h"
#include "../DialogComponent.h"
#include "../GUI/Window_Area.h"

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
	
	m_pgd.clear();
	
	for (ska::EntityId entityId : m_processed) {
		ska::GraphicComponent& gc = m_entityManager.getComponent<ska::GraphicComponent>(entityId);
		ska::PositionComponent& pos = m_entityManager.getComponent<ska::PositionComponent>(entityId);
		const int relPosX = pos.x - cameraX;
		const int relPosY = pos.y - cameraY - pos.z;
		for (auto& sprite : gc.sprite) {
			if (!(relPosX + sprite.getWidth() < 0 || camera != NULL && relPosX >= camera->w ||
				relPosY + sprite.getHeight() < 0 || camera != NULL && relPosY >= camera->h)) {
				m_pgd.push_back(ska::PositionnedGraphicDrawable(sprite, relPosX, relPosY, pos.y + (camera == NULL ? 0 : camera->h*pos.z), pos.y));
			}
		}

		if (m_entityManager.hasComponent<DialogComponent>(entityId)) {
			m_drawables->add(m_entityManager.getComponent<DialogComponent>(entityId).dialog);
		}
	}
	for (auto& pgd : m_pgd) {
		m_drawables->add(pgd);
	}

	m_drawables = NULL;
}

GraphicSystem::~GraphicSystem() {
}
