#include "CameraFixedSystem.h"
#include "../Rectangle.h"
#include "../GraphicComponent.h"

ska::CameraFixedSystem::CameraFixedSystem(EntityManager& entityManager, const unsigned int screenW, const unsigned int screenH, Point<int> point) : 
CameraSystem(entityManager, screenW, screenH),
m_fixed(point) {
	m_pos = &m_fixed;
}

void ska::CameraFixedSystem::refresh() {
	focusOn(*m_pos, NULL);
}

ska::CameraFixedSystem::~CameraFixedSystem()
{
}
