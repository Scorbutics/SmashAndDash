#include "Game/__internalConfig/LoggerConfig.h"
#include <iostream>
#include <string>
#include "Bar.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/ECS/EntityManager.h"
#include "../../Utils/IDs.h"
#include "../../Gameplay/Data/RawStatistics.h"
#include "Core/ECS/EntityManager.h"
#include "Base/Patterns/Observable.h"
#include "Graphic/System/CameraSystem.h"
#include "Graphic/GraphicComponent.h"
#include <cmath>
#include "Core/Draw/Renderer.h"

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

void Bar::render(ska::Renderer& renderer) const {
	if (!m_visible) {
		return;
	}

	ska::Rectangle rectSize{ 0, 0, m_barSize.w, m_barSize.h };

	const auto& camera = m_cameraSystem.getDisplay();

	auto pos = m_entityManager.getComponent<ska::PositionComponent>(m_entityId);
	// Drawn over the sprite : between the logic positions, as the sprite (ska::FrameClock)
	if (m_entityManager.hasComponent<ska::GraphicComponent>(m_entityId)) {
		const auto drawn = m_entityManager.getComponent<ska::GraphicComponent>(m_entityId).drawnPosition(pos);
		pos.x = static_cast<long>(std::lround(drawn.x));
		pos.y = static_cast<long>(std::lround(drawn.y));
		pos.z = static_cast<long>(std::lround(drawn.z));
	}
	const int offsetY = - pos.z;

	renderer.render(m_barStyle, pos.x - camera.x, pos.y + offsetY - camera.y, nullptr);
	renderer.render(m_barContent, pos.x + m_barSize.x - camera.x, pos.y + m_barSize.y + offsetY - camera.y, &rectSize);

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
