#include "ClickEvent.h"


ska::ClickEvent::ClickEvent(const ska::MouseEventType& state, ska::Point<int>& pos) : HoverEvent(state, pos, pos) {
}

