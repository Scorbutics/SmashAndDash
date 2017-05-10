#include "HoverEventListener.h"
#include "ClickEvent.h"

ska::HoverEventListener::HoverEventListener() :
EventListener<HoverEvent>() {

}

bool ska::HoverEventListener::notify(Widget& tthis, HoverEvent& e) {
	return EventListener<HoverEvent>::notify(tthis, e);

	/*if (!EventListener<HoverEvent>::notify(tthis, e)) {
		//Evenement géré par le widget. On le stoppe totalement
		e.stopPropagation(StopType::STOP_WIDGET);
	}
	return true; */

	/*auto result = EventListener<HoverEvent>::notify(tthis, e);
	e.stopPropagation(StopType::STOP_WIDGET);
	return result;*/
}
