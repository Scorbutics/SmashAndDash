#pragma once

#include "EventListener.h"


namespace ska {
	template <class T>
	class ValueChangedEvent;

	template <class T>
	class ValueChangedEventListener : public EventListener<ValueChangedEvent<T>> {
	public:
		ValueChangedEventListener() : 
			EventListener<ValueChangedEvent<T>>() {
		}

		virtual ~ValueChangedEventListener() = default;
	};
}
