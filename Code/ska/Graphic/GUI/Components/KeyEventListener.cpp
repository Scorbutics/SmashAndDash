#include "KeyEventListener.h"
#include "Widget.h"
#include "KeyEvent.h"

ska::KeyEventListener::KeyEventListener(Widget& tthis) : 
	ska::EventListener<KeyEvent>(tthis) {

}

bool ska::KeyEventListener::notify(Widget& tthis, KeyEvent& e) {
	EventListener<KeyEvent>::notify(tthis, e);
	/* Evenement géré par le widget. On le stoppe totalement */
	e.stopPropagation(StopType::STOP_WIDGET);
	return true;
}