#include "GUIBattle.h"
#include "Bar.h"
#include "../../Gameplay/Data/RawStatistics.h"
#include "../../ska/Graphic/Draw/DrawableContainer.h"

GUIBattle::GUIBattle(StatisticsChangeObservable& obs) :
StatisticsChangeObserver(std::bind(&GUIBattle::onStatisticsChange, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
m_statsObservable(obs) {
	m_statsObservable.addObserver(*this);
}

void GUIBattle::onStatisticsChange(const ska::EntityId& target, RawStatistics<int>& targetStats, const ska::EntityId& src) {
	if (m_bars.find(target) != m_bars.end()) {
		m_bars[target]->setCurrentValue(targetStats.hp);
	}
}

void GUIBattle::addHPBar(ska::CameraSystem& camSys, unsigned int maxValue, unsigned int currentValue, ska::EntityManager& em, const ska::EntityId& entityId) {
	m_bars[entityId] = std::move(BarPtr(new Bar(camSys, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"hpbar.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"hpbarcontent.png", maxValue, em, entityId)));
	m_bars[entityId]->setCurrentValue(currentValue);
}

void GUIBattle::graphicUpdate(ska::DrawableContainer& drawables) {
	for (auto& b : m_bars) {
		drawables.add(*b.second);
	}
}

void GUIBattle::clear() {
	m_bars.clear();
}

GUIBattle::~GUIBattle() {
	m_statsObservable.removeObserver(*this);
	clear();
}