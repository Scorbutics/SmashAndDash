#pragma once
#include "WidgetEvent.h"
#include "../../Point.h"

#include "MouseEventType.h"

namespace ska {
	class Widget;
	class FocusEventListener;

	class FocusEvent : public WidgetEvent<FocusEvent, FocusEventListener> {
	public:
		FocusEvent(const MouseEventType& state);
		MouseEventType getState() const;
		virtual unsigned int getMask() const override;
		virtual bool affects(const Widget& w) const override;
		virtual ~FocusEvent() = default;
	
	private:
		MouseEventType m_state;
	};
}