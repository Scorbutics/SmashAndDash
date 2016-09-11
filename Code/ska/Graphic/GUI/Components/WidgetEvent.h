#pragma once
#include "ButtonState.h"
#include "../../Point.h"

namespace ska {

	class Widget;
	class WidgetEvent {
	public:
		WidgetEvent();
		void setTarget(Widget* t);
		Widget* getTarget();
		virtual ~WidgetEvent() = default;

	private:
		Widget* m_target;

	};
}