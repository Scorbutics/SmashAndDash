#pragma once
#include <functional>
#include "FocusEvent.h"
#include "EventListener.h"

namespace ska {
	class Widget;

	/* Handler triggered when a key is pressed */
	using FocusEventHandler = std::function<void(Widget*, FocusEvent&)>;

	class FocusEventListener : public EventListener<FocusEvent> {
	public:
		explicit FocusEventListener(Widget& tthis);
		virtual bool notify(Widget& tthis, FocusEvent& e) override;
		virtual ~FocusEventListener() = default;

	};
}
