#include "Widget.h"
#include "FocusEvent.h"
#include "WidgetMaskHelper.h"
#include "FocusEventListener.h"

ska::FocusEvent::FocusEvent(const MouseEventType& state) : 
	m_state(state) {

}

ska::MouseEventType ska::FocusEvent::getState() const {
	return m_state;
}

unsigned int ska::FocusEvent::getMask() const {
	return WidgetMaskHelper::template getMask<FocusEventListener>();
}

bool ska::FocusEvent::affects(const Widget& w) const {
	if (stopped() != NOT_STOPPED) {
		return false;
	}

	if (!w.isVisible()) {
		return false;
	}
	return true;
}

