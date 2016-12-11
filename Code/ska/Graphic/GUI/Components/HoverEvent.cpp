#include "HoverEvent.h"
#include "Widget.h"
#include "WidgetMaskHelper.h"

ska::HoverEvent::HoverEvent(const MouseEventType& state, const Point<int>& pos, const Point<int>& mousePos, const Point<int>& relativeMousePos) : 
	m_state(state), m_pos(pos), m_mousePos(mousePos), m_relativeMousePos(relativeMousePos) {

}

const ska::MouseEventType& ska::HoverEvent::getState() const {
	return m_state;
}

const ska::Point<int>& ska::HoverEvent::getPosition() const {
	return m_pos;
}

const ska::Point<int>& ska::HoverEvent::getRelativeMousePosition() const {
	return m_relativeMousePos;
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
