#include "HoverEvent.h"
#include "Utils/RectangleUtils.h"
#include "../Utils/WidgetMaskHelper.h"
#include "../Components/Widget.h"

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

ska::Point<int> ska::HoverEvent::getPosition(const Widget& w) const {
	return m_pos - w.getAbsolutePosition();
}

bool ska::HoverEvent::affects(const Widget& w) const {
	if(stopped() != NOT_STOPPED) {
		return false;
	}

	if (!w.isVisible()) {
		return false;
	}
	const auto& relativeEventPos = getPosition() - w.getAbsolutePosition();
	return RectangleUtils::isPositionInBox(relativeEventPos, Rectangle{ 0, 0, w.getBox().w, w.getBox().h });
}

unsigned int ska::HoverEvent::getMask() const {
	return WidgetMaskHelper::getMask<HoverEventListener>();
}
