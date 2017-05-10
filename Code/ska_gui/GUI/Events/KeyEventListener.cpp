#include "KeyEventListener.h"
#include "KeyEvent.h"

ska::KeyEventListener::KeyEventListener() : 
	EventListener<KeyEvent>() {

}

bool ska::KeyEventListener::notify(Widget& tthis, KeyEvent& e) {
	EventListener<KeyEvent>::notify(tthis, e);
	/* Evenement géré par le widget. On le stoppe totalement */
	e.stopPropagation(STOP_WIDGET);
	return true;
}
