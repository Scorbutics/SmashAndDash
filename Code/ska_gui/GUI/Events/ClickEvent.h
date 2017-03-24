#pragma once

#include "Point.h"

#include "HoverEvent.h"

namespace ska {
	class ClickEvent : public HoverEvent {
	public:
		ClickEvent(const MouseEventType& state, Point<int>& pos);
		~ClickEvent() = default;

		virtual unsigned int getMask() const override;
	};
}
