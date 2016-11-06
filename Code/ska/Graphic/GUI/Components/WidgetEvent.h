#pragma once
#include "ButtonState.h"
#include "../../Point.h"

namespace ska {

	enum StopType {
		NOT_STOPPED,
		STOP_CALLBACK,
		STOP_WIDGET
	};

	class Widget;
	class WidgetEvent {
	public:
		WidgetEvent();
		void setTarget(Widget* t);
		void stopPropagation(StopType s);
		StopType stopped() const;
		Widget* getTarget();
		Widget const* getCTarget() const;
		virtual bool affects(const Widget& w) const = 0;
		virtual ~WidgetEvent() = default;

	private:
		Widget* m_target;
		StopType m_stopPropagation;

	};
}