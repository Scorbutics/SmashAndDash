#include "ClickEvent.h"
#include "ClickEventListener.h"
#include "../Utils/WidgetMaskHelper.h"

ska::ClickEvent::ClickEvent(const MouseEventType& state, Point<int>& pos) : 
	HoverEvent(state, pos, pos, pos - pos) {
}


unsigned int ska::ClickEvent::getMask() const {
	return WidgetMaskHelper::getMask<ClickEventListener>();
}
