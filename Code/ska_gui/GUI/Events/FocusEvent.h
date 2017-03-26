#pragma once
#include "WidgetEvent.h"
#include "MouseEventType.h"

namespace ska {
	class Widget;
	class FocusEventListener;

	class FocusEvent : public WidgetEvent<FocusEvent, FocusEventListener> {
	public:
		FocusEvent(Widget* clickedTarget, const MouseEventType& state);

		const Widget* getClickedTarget() const;

		MouseEventType getState() const;
		virtual unsigned int getMask() const override;
		virtual bool affects(const Widget& w) const override;
		virtual ~FocusEvent() = default;
	
	private:
		MouseEventType m_state;
		Widget* m_clickedTarget;
	};
}
