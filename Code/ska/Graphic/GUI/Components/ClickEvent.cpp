#include "ClickEvent.h"


ska::ClickEvent::ClickEvent(ska::Point<int>& pos) : m_clickPosition(pos) {
}

const ska::Point<int>& ska::ClickEvent::getClickPosition() const {
	return m_clickPosition;
}
