#pragma once
#include "ButtonState.h"
#include "../../Point.h"

namespace ska {
	enum MouseEventType {
		MOUSE_HOVER,
		MOUSE_OUT,
		MOUSE_CLICK,
		MOUSE_RELEASE
	};
	class Widget;
	class HoverEvent {
	public:
		HoverEvent(const MouseEventType& state, const Point<int>& pos);
		const MouseEventType& getState() const;
		const ska::Point<int>& getPosition() const;
		const ska::Point<int> getPosition(const ska::Widget& w) const;
		bool isOn(const Widget& w) const;
		virtual ~HoverEvent() = default;

	private:
		MouseEventType m_state;
		Point<int> m_pos;

		bool m_alreadyTriggered;

	};
}