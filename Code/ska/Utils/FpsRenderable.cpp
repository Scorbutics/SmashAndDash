#include "FpsRenderable.h"
#include "StringUtils.h"
#include "TimeUtils.h"

ska::FpsRenderable::FpsRenderable() : 
	m_toggleVisible(true), m_t0(0) {

}

void ska::FpsRenderable::update(const double value) {
	if (ska::TimeUtils::getTicks() - m_t0 > DELAY) {
		m_fps.loadFromText(34, ska::StringUtils::intToStr(value), ska::Color(255, 0, 0, 0));
		m_t0 = ska::TimeUtils::getTicks();
	}
}

bool ska::FpsRenderable::isVisible() const {
	return m_toggleVisible;
}

void ska::FpsRenderable::setVisible(bool v) {
	m_toggleVisible = v;
}

void ska::FpsRenderable::display() const {
	m_fps.render(0, 0);
}