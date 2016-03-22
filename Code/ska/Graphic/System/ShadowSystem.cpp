#include "ShadowSystem.h"
#include "../SpritePath.h"

ska::ShadowSystem::ShadowSystem(ska::CameraSystem& camera, ska::EntityManager& entityManager) : System(entityManager), AbstractGraphicSystem(camera) {
	m_shadow.load(SpritePath::getInstance().getPath(SPRITEBANK_ANIMATION, 13), 2, 1, 1, false, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 127);
}

void ska::ShadowSystem::refresh() {
	const ska::Rectangle* camera = m_camera.getDisplay();
	const unsigned int cameraX = (camera == NULL || camera->x < 0 ? 0 : camera->x);
	const unsigned int cameraY = (camera == NULL || camera->y < 0 ? 0 : camera->y);
	
	if (m_drawables == NULL) {
		return;
	}

	m_pgd.clear();
	m_pgd.reserve(m_processed.size());

	for (ska::EntityId entityId : m_processed) {
		PositionComponent& pos = m_entityManager.getComponent<ska::PositionComponent>(entityId);
		HitboxComponent& hc = m_entityManager.getComponent<ska::HitboxComponent>(entityId);

		const int relPosX = pos.x + hc.xOffset - cameraX;
		const int relPosY = pos.y + hc.yOffset - cameraY - m_shadow.getHeight()/3;
		const int priority = pos.y + (camera == NULL ? 0 : camera->h*pos.z) - 10;
		if (!(relPosX + m_shadow.getWidth() < 0 || camera != NULL && relPosX >= camera->w ||
			relPosY + m_shadow.getHeight() < 0 || camera != NULL && relPosY >= camera->h)) {
			m_pgd.push_back(PositionnedGraphicDrawable(m_shadow, relPosX, relPosY, priority, priority));
			m_drawables->add(m_pgd[m_pgd.size() - 1]);
		}
	}

}

ska::ShadowSystem::~ShadowSystem() {
}
