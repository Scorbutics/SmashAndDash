#pragma once
#include <functional>
#include "FocusEvent.h"
#include "EventListener.h"

namespace ska {
	class Widget;

	/* Handler triggered when a key is pressed */
	using FocusEventHandler = std::function<void(ska::Widget*, ska::FocusEvent&)>;

	class FocusEventListener : public EventListener<FocusEvent> {
	public:
		FocusEventListener(Widget& tthis);
		virtual bool notify(Widget& tthis, FocusEvent& e) override;
		virtual ~FocusEventListener() = default;

	private:

	};
}