#pragma once
#include "WidgetEvent.h"
#include "../../Point.h"
#include "HoverEventListener.h"

namespace ska {
	enum MouseEventType {
		MOUSE_ENTER,
		MOUSE_OUT,
		MOUSE_OVER,
		MOUSE_CLICK,
		MOUSE_RELEASE
	};

	class Widget;

	class HoverEvent : public WidgetEvent<HoverEvent, HoverEventListener> {
	public:
		HoverEvent(const MouseEventType& state, const Point<int>& pos, const Point<int>& mousePos);
		const MouseEventType& getState() const;
		const ska::Point<int>& getPosition() const;
		const ska::Point<int>& getMousePosition() const;
		const ska::Point<int> getPosition(const ska::Widget& w) const;
		
		virtual unsigned int getMask() const override;
		virtual bool affects(const Widget& w) const override;

		virtual ~HoverEvent() = default;

	private:
		MouseEventType m_state;
		Point<int> m_pos;
		Point<int> m_mousePos;

	};
}