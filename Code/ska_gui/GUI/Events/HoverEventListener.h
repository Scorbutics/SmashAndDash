#pragma once
#include <functional>

#include "EventListener.h"

namespace ska {
	class Widget;
	class HoverEvent;

	/* Handler triggered when hovering on the widget */
	using HoverEventHandler = std::function<void(Widget*, HoverEvent&)>;

	class HoverEventListener : public EventListener<HoverEvent> {
	public:
		HoverEventListener();
		virtual bool notify(Widget& tthis, HoverEvent& e) override;
		virtual ~HoverEventListener() = default;

	};
}
