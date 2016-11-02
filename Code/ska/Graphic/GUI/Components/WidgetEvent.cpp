#include "WidgetEvent.h"


ska::WidgetEvent::WidgetEvent() : m_target(nullptr) {

}

void ska::WidgetEvent::setTarget(Widget* t) {
	m_target = t;
}

ska::Widget* ska::WidgetEvent::getTarget() {
	return m_target;
}

ska::Widget const* ska::WidgetEvent::getCTarget() const {
	return m_target;
}

