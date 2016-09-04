#include "HoverEvent.h"
#include "Widget.h"
#include "../../../Utils/RectangleUtils.h"

ska::HoverEvent::HoverEvent(const MouseEventType& state, const ska::Point<int>& pos) : m_state(state), m_pos(pos), m_alreadyTriggered(false) {

}

const ska::MouseEventType& ska::HoverEvent::getState() const {
	return m_state;
}

const ska::Point<int>& ska::HoverEvent::getPosition() const {
	return m_pos;
}

const ska::Point<int> ska::HoverEvent::getPosition(const ska::Widget& w) const {
	return m_pos - w.getAbsolutePosition();
}

bool ska::HoverEvent::isOn(const Widget& w) const {
	auto& relativeEventPos = getPosition(w);
	return ska::RectangleUtils::isPositionInBox(relativeEventPos, ska::Rectangle{ 0, 0, w.getBox().w, w.getBox().h });
}

