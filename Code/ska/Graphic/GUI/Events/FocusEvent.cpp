#include "FocusEvent.h"
#include "FocusEventListener.h"
#include "../Utils/WidgetMaskHelper.h"
#include "../Components/Widget.h"

ska::FocusEvent::FocusEvent(Widget* clickedTarget, const MouseEventType& state) : 
	m_state(state),
	m_clickedTarget(clickedTarget) {

}

const ska::Widget* ska::FocusEvent::getClickedTarget() const {
	return m_clickedTarget;
}

ska::MouseEventType ska::FocusEvent::getState() const {
	return m_state;
}

unsigned int ska::FocusEvent::getMask() const {
	return WidgetMaskHelper::getMask<FocusEventListener>();
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

