#pragma once

#include "EventListener.h"


namespace ska {
	template <class T>
	class ValueChangedEvent;

	template <class T>
	class ValueChangedEventListener : public EventListener<ValueChangedEvent<T>> {
	public:
		explicit ValueChangedEventListener(Widget& tthis) : 
			EventListener<ValueChangedEvent<T>>(tthis) {
		}

		virtual ~ValueChangedEventListener() = default;
	};
}
