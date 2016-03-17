#include "GraphicSystem.h"
#include "../PositionnedGraphicDrawable.h"

ska::GraphicSystem::GraphicSystem(ska::CameraSystem& camera, ska::EntityManager& entityManager) : System(entityManager), m_camera(camera) {
	m_drawables = NULL;
}

void ska::GraphicSystem::refresh() {
	const ska::Rectangle* camera = m_camera.getDisplay();
	const unsigned int cameraX = (camera == NULL ? 0 : camera->x);
	const unsigned int cameraY = (camera == NULL ? 0 : camera->y);

	if (m_drawables == NULL) {
		return;
	}
	
	m_pgd.clear();
	m_pgd.reserve(m_processed.size());
	
	for (ska::EntityId entityId : m_processed) {
		GraphicComponent& gc = m_entityManager.getComponent<GraphicComponent>(entityId);
		ska::PositionComponent& pos = m_entityManager.getComponent<ska::PositionComponent>(entityId);
		const int relPosX = pos.x - cameraX;
		const int relPosY = pos.y - cameraY;
		if (!(relPosX + gc.sprite.getWidth() < 0 || camera != NULL && relPosX >= camera->w ||
			relPosY + gc.sprite.getHeight() < 0 || camera != NULL && relPosY >= camera->h)) {
			m_pgd.push_back(PositionnedGraphicDrawable(gc, relPosX, relPosY, relPosY + pos.z));
			m_drawables->add(m_pgd[m_pgd.size() - 1]);
		}
	}

	m_drawables = NULL;
}

void ska::GraphicSystem::setDrawables(DrawableContainer& container) {
	m_drawables = &container;
}

ska::GraphicSystem::~GraphicSystem() {
}
