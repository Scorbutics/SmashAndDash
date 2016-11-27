#include "FocusEventListener.h"
#include "Widget.h"
#include "FocusEvent.h"

ska::FocusEventListener::FocusEventListener(Widget& tthis) :
ska::EventListener<FocusEvent>(tthis) {

}

bool ska::FocusEventListener::notify(Widget& tthis, FocusEvent& e) {
	const auto& result = EventListener<FocusEvent>::notify(tthis, e);
	/* Evenement g�r� par le widget. On le stoppe totalement */
	//e.stopPropagation(StopType::STOP_WIDGET);
	return result;
}
