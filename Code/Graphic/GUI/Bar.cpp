#include <iostream>
#include <string>
#include "Bar.h"
#include "../../ska/ECS/ECSDefines.h"
#include "../../ska/ECS/EntityManager.h"
#include "../../Utils/IDs.h"
#include "../../Gameplay/Data/RawStatistics.h"
#include "../../ska/ECS/EntityManager.h"
#include "../../ska/Utils/Observable.h"
#include "../../ska/Graphic/System/CameraSystem.h"

Bar::Bar(StatisticsChangeObservable& obs, ska::CameraSystem& cam, const std::string& barStyleName, const std::string& barContentName, int maxValue, ska::EntityManager& em, const ska::EntityId& entityId) :
StatisticsChangeObserver(std::bind(&Bar::onStatisticsChange, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
m_barStyle(barStyleName),
m_barContent(barContentName),
m_entityManager(em),
m_maxValue(maxValue),
m_observable(obs),
m_cameraSystem(cam),
m_entityId(entityId) {
        
	const ska::PositionComponent& pos = m_entityManager.getComponent<ska::PositionComponent>(entityId);
    m_barSize.w = (int)((m_curValue * ((m_barStyle.getWidth() * 5) /6.)) / m_maxValue);
    m_barSize.h = (int)((m_barStyle.getHeight() * 5) /6);
	m_barSize.x = m_barStyle.getWidth() / 12;
	m_barSize.y = m_barStyle.getHeight() / 12;

    m_visible = true;

	m_observable.addObserver(*this);
	setPriority(GUI_DEFAULT_DISPLAY_PRIORITY);
}

void Bar::display() const {
	if (!m_visible) {
		return;
	}

	ska::Rectangle rectSize{ 0, 0, m_barSize.w, m_barSize.h };

	const ska::Rectangle* camera = m_cameraSystem.getDisplay();
	const ska::Rectangle nonNullCamera = camera == nullptr ? ska::Rectangle{ 0 } : *camera;


	const ska::PositionComponent& pos = m_entityManager.getComponent<ska::PositionComponent>(m_entityId);
	const int offsetY = - pos.z;

	m_barStyle.render(pos.x - nonNullCamera.x, pos.y + offsetY - nonNullCamera.y);
	m_barContent.render(pos.x + m_barSize.x - nonNullCamera.x, pos.y + m_barSize.y + offsetY - nonNullCamera.y, &rectSize);
	
}

void Bar::onStatisticsChange(const ska::EntityId& target, RawStatistics<int>& targetStats, const ska::EntityId& src) {
	//TODO : observateur = GUI et non pas Bar directement
	if (m_entityId == target) {
		setCurrentValue(targetStats.hp);
	}
}

void Bar::setCurrentValue(unsigned int v) {
    m_curValue = v;

    m_barSize.w = (int)((m_curValue * ((m_barStyle.getWidth() * 5) /6.)) / m_maxValue);
}

bool Bar::isVisible() const {
    return m_visible;
}

void Bar::setVisible(bool x) {
    m_visible = x;
}

Bar::~Bar() {
	m_observable.removeObserver(*this);
}