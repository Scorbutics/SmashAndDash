#include "ClickEvent.h"


ska::ClickEvent::ClickEvent(const ska::MouseEventType& state, ska::Point<int>& pos) : HoverEvent(state), m_clickPosition(pos) {
}

const ska::Point<int>& ska::ClickEvent::getClickPosition() const {
	return m_clickPosition;
}

