#include "WidgetEvent.h"


ska::WidgetEvent::WidgetEvent() : m_target(nullptr) {

}

void ska::WidgetEvent::setTarget(Widget* t) {
	m_target = t;
}

ska::Widget* ska::WidgetEvent::getTarget() {
	return m_target;
}

