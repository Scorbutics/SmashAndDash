#include <iostream>
#include <string>
#include "Bar.h"
#include "ECS/ECSDefines.h"
#include "ECS/EntityManager.h"
#include "../../Utils/IDs.h"
#include "../../Gameplay/Data/RawStatistics.h"
#include "ECS/EntityManager.h"
#include "Utils/Observable.h"
#include "Graphic/System/CameraSystem.h"

Bar::Bar(ska::CameraSystem& cam, const std::string& barStyleName, const std::string& barContentName, int maxValue, ska::EntityManager& em, const ska::EntityId& entityId) :
m_entityManager(em),
m_entityId(entityId),
m_cameraSystem(cam),
m_barStyle(barStyleName),
m_barContent(barContentName),
m_maxValue(maxValue),
m_curValue(maxValue),
m_visible(true) {

    m_barSize.w = static_cast<int>((m_curValue * ((m_barStyle.getWidth() * 5) /6.)) / m_maxValue);
    m_barSize.h = static_cast<int>((m_barStyle.getHeight() * 5) /6);
	m_barSize.x = m_barStyle.getWidth() / 12;
	m_barSize.y = m_barStyle.getHeight() / 12;

	DrawableFixedPriority::setPriority(GUI_DEFAULT_DISPLAY_PRIORITY);
}

void Bar::display() const {
	if (!m_visible) {
		return;
	}

	ska::Rectangle rectSize{ 0, 0, m_barSize.w, m_barSize.h };

	const ska::Rectangle* camera = m_cameraSystem.getDisplay();
	const ska::Rectangle nonNullCamera = camera == nullptr ? ska::Rectangle{ 0, 0, 0, 0 } : *camera;


	const ska::PositionComponent& pos = m_entityManager.getComponent<ska::PositionComponent>(m_entityId);
	const int offsetY = - pos.z;

	m_barStyle.render(pos.x - nonNullCamera.x, pos.y + offsetY - nonNullCamera.y);
	m_barContent.render(pos.x + m_barSize.x - nonNullCamera.x, pos.y + m_barSize.y + offsetY - nonNullCamera.y, &rectSize);

}

void Bar::setCurrentValue(unsigned int v) {
    m_curValue = v;

    m_barSize.w = static_cast<int>((m_curValue * ((m_barStyle.getWidth() * 5) / 6.)) / m_maxValue);
}

bool Bar::isVisible() const {
    return m_visible;
}

void Bar::setVisible(bool x) {
    m_visible = x;
}

Bar::~Bar() {
}
