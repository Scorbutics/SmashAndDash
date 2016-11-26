#include "ClickEvent.h"
#include "WidgetMaskHelper.h"
#include "ClickEventListener.h"
ska::ClickEvent::ClickEvent(const ska::MouseEventType& state, ska::Point<int>& pos) : 
	HoverEvent(state, pos, pos),
	m_pushToFront(false) {
}


unsigned int ska::ClickEvent::getMask() const {
	return WidgetMaskHelper::template getMask<ClickEventListener>();
}
