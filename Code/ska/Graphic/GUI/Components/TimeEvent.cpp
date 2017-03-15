#include "TimeEvent.h"
#include "WidgetMaskHelper.h"

unsigned ska::TimeEvent::getMask() const {
	return WidgetMaskHelper::getMask<TimeEventListener>();
}

bool ska::TimeEvent::affects(const Widget& w) const {
	return true;
}

ska::StopType ska::TimeEvent::stopped() const {
	return NOT_STOPPED;
}
