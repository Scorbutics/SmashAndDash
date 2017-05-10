#include "FocusEventListener.h"
#include "FocusEvent.h"

ska::FocusEventListener::FocusEventListener() :
EventListener<FocusEvent>() {

}

bool ska::FocusEventListener::notify(Widget& tthis, FocusEvent& e) {
	const auto& result = EventListener<FocusEvent>::notify(tthis, e);
	/* Evenement géré par le widget. On le stoppe totalement */
	//e.stopPropagation(StopType::STOP_WIDGET);
	return result;
}
