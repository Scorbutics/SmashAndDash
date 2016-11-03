#include "HoverEvent.h"
#include "Widget.h"

ska::HoverEvent::HoverEvent(const MouseEventType& state, const ska::Point<int>& pos, const ska::Point<int>& mousePos) : 
	m_state(state), m_pos(pos), m_mousePos(mousePos) {

}

const ska::MouseEventType& ska::HoverEvent::getState() const {
	return m_state;
}

const ska::Point<int>& ska::HoverEvent::getPosition() const {
	return m_pos;
}

const ska::Point<int>& ska::HoverEvent::getMousePosition() const {
	return m_mousePos;
}

const ska::Point<int> ska::HoverEvent::getPosition(const ska::Widget& w) const {
	return m_pos - w.getAbsolutePosition();
}

bool ska::HoverEvent::affects(const Widget& w) const {
	return !stopped() && w.isAffectedBy(*this);
}

