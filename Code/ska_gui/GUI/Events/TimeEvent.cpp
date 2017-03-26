#include "TimeEvent.h"
#include "TimeEventListener.h"
#include "../Utils/WidgetMaskHelper.h"

unsigned ska::TimeEvent::getMask() const {
	return WidgetMaskHelper::getMask<TimeEventListener>();
}

bool ska::TimeEvent::affects(const Widget& ) const {
	return true;
}

ska::StopType ska::TimeEvent::stopped() const {
	return NOT_STOPPED;
}
