#include "ClickEvent.h"
#include "WidgetMaskHelper.h"
#include "ClickEventListener.h"
ska::ClickEvent::ClickEvent(const MouseEventType& state, Point<int>& pos) : 
	HoverEvent(state, pos, pos, pos - pos),
	m_pushToFront(false) {
}


unsigned int ska::ClickEvent::getMask() const {
	return WidgetMaskHelper::template getMask<ClickEventListener>();
}
