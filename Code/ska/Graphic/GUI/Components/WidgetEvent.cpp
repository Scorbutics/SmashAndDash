#include "WidgetEvent.h"


ska::WidgetEvent::WidgetEvent() : m_target(nullptr), m_stopPropagation(NOT_STOPPED) {

}

void ska::WidgetEvent::setTarget(Widget* t) {
	m_target = t;
}

void ska::WidgetEvent::stopPropagation(ska::StopType st) {
	m_stopPropagation = st;
}

ska::StopType ska::WidgetEvent::stopped() const {
	return m_stopPropagation;
}

ska::Widget* ska::WidgetEvent::getTarget() {
	return m_target;
}

ska::Widget const* ska::WidgetEvent::getCTarget() const {
	return m_target;
}

