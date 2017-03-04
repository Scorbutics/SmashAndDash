#pragma once

#include "ValueChangedEventListener.h"
#include "WidgetEvent.h"

namespace ska {
	
	template <class T>
	class ValueChangedEvent : public WidgetEvent<ValueChangedEvent<T>, ValueChangedEventListener<T>> {
	public:
		ValueChangedEvent(T oldValue, T value) :
			m_oldValue(oldValue), m_value(value) {
		}

		void operator=(const ValueChangedEvent&) = delete;

		const T& getValue() const {
			return m_value;
		}
		
		const T& getOldValue() const {
			return m_oldValue;
		}
		
		virtual unsigned int getMask() const override {
			return WidgetMaskHelper::template getMask<ValueChangedEventListener<T>>();
		}

		virtual bool affects(const Widget&) const override {
			if (stopped() != NOT_STOPPED) {
				return false;
			}
			return true;
		}

		virtual ~ValueChangedEvent() = default;

	private:
		const T m_oldValue;
		const T m_value;
	};
}