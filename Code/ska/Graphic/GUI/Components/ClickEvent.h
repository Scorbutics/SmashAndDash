#pragma once

#include "../../Point.h"

#include "HoverEvent.h"

namespace ska {
	class ClickEvent : public HoverEvent {
	public:
		ClickEvent(const ska::MouseEventType& state, ska::Point<int>& pos);
		~ClickEvent() = default;		
	};
}