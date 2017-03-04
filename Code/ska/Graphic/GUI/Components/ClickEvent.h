#pragma once

#include "../../Point.h"

#include "HoverEvent.h"

namespace ska {
	class ClickEvent : public HoverEvent {
	public:
		ClickEvent(const MouseEventType& state, Point<int>& pos);
		~ClickEvent() = default;

		void pushWidgetToFront();

		virtual unsigned int getMask() const override;
	private:
		bool m_pushToFront;
	};
}