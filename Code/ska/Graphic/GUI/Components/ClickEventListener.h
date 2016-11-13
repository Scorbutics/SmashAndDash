#pragma once
#include <functional>

#include "ClickEvent.h"
#include "EventListener.h"

namespace ska {

	/* Handler triggered when a click occurs on the widget */
	using ClickEventHandler = std::function<void(ska::Widget*, ska::ClickEvent&)>;
	class ClickEventListener : public EventListener<ClickEvent> {
	public:
		ClickEventListener(Widget& tthis);
		virtual bool notify(Widget& tthis, ClickEvent& e) override;
		virtual ~ClickEventListener() = default;

	private:

	};
}