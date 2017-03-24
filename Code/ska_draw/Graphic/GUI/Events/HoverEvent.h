#pragma once
#include "WidgetEvent.h"
#include "Point.h"
#include "HoverEventListener.h"
#include "MouseEventType.h"

namespace ska {

	class Widget;

	class HoverEvent : public WidgetEvent<HoverEvent, HoverEventListener> {
	public:
		HoverEvent(const MouseEventType& state, const Point<int>& pos, const Point<int>& mousePos, const Point<int>& relativeMousePos);
		const MouseEventType& getState() const;
		const Point<int>& getPosition() const;

		const Point<int>& getRelativeMousePosition() const;
		const Point<int>& getMousePosition() const;
		Point<int> getPosition(const Widget& w) const;

		virtual unsigned int getMask() const override;
		virtual bool affects(const Widget& w) const override;

		virtual ~HoverEvent() = default;

	private:
		MouseEventType m_state;
		Point<int> m_pos;
		Point<int> m_mousePos;
		Point<int> m_relativeMousePos;

	};
}
