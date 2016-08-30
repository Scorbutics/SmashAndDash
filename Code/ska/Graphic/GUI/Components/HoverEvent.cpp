#include "HoverEvent.h"

ska::HoverEvent::HoverEvent(const MouseEventType& state) : m_state(state) {

}

const ska::MouseEventType& ska::HoverEvent::getState() const {
	return m_state;
}

