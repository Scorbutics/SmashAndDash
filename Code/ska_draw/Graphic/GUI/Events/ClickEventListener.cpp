#include "ClickEventListener.h"
#include "ClickEvent.h"

ska::ClickEventListener::ClickEventListener(Widget& tthis) :
EventListener<ClickEvent>(tthis) {

}

bool ska::ClickEventListener::notify(Widget& tthis, ClickEvent& e) {
	const auto& result = EventListener<ClickEvent>::notify(tthis, e);
	/* Evenement géré par le widget. On le stoppe totalement */
	//e.stopPropagation(StopType::STOP_WIDGET);
	return result;
}
