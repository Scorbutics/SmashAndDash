#pragma once
#include <functional>

#include "EventListener.h"
#include "TimeEvent.h"

namespace ska {
	class Widget;
	
	using TimeEventHandler = std::function<void(Widget*, TimeEvent&)>;
	class TimeEventListener : public EventListener<TimeEvent> {
	public:
		explicit TimeEventListener(Widget& tthis) : EventListener<TimeEvent>(tthis) {
		}

		virtual bool notify(Widget& tthis, TimeEvent& e) override {
			const auto& result = EventListener<TimeEvent>::notify(tthis, e);
			return result;
		}

		virtual ~TimeEventListener() = default;

	private:

	};
}
