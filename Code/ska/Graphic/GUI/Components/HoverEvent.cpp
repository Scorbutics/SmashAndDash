#include "HoverEvent.h"
#include "Widget.h"
#include "WidgetMaskHelper.h"

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
	if(stopped() != NOT_STOPPED) {
		return false;
	}
	
	if (!w.isVisible()) {
		return false;
	}
	const ska::Point<int>& relativeEventPos = getPosition() - w.getAbsolutePosition();
	return ska::RectangleUtils::isPositionInBox(relativeEventPos, ska::Rectangle{ 0, 0, w.getBox().w, w.getBox().h });
}

unsigned int ska::HoverEvent::getMask() const {
	return WidgetMaskHelper::template getMask<HoverEventListener>();
}
