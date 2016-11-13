#include "HoverEventListener.h"
#include "Widget.h"
#include "ClickEvent.h"

ska::HoverEventListener::HoverEventListener(Widget& tthis) :
ska::EventListener<HoverEvent>(tthis) {

}

bool ska::HoverEventListener::notify(Widget& tthis, HoverEvent& e) {
	/*if (!EventListener<HoverEvent>::notify(tthis, e)) {
		//Evenement géré par le widget. On le stoppe totalement
		e.stopPropagation(StopType::STOP_WIDGET);
	}
	return true; */
	return EventListener<HoverEvent>::notify(tthis, e);
}
