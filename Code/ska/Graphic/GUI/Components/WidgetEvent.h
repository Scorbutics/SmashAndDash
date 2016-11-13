#pragma once
#include "../../Point.h"
#include "IWidgetEvent.h"

namespace ska {

	class Widget;

	template <class ...HL>
	class HandledWidget;

	template <class E, class L>
	class WidgetEvent : public IWidgetEvent {
	public:
		WidgetEvent() : m_target(nullptr), m_stopPropagation(NOT_STOPPED) {
		}

		void setTarget(Widget* t) {
			m_target = t;
		}

		void stopPropagation(StopType st) {
			m_stopPropagation = st;
		}

		StopType stopped() const {
			return m_stopPropagation;
		}

		Widget* getTarget() {
			return m_target;
		}

		Widget const* getCTarget() const {
			return m_target;
		}

		virtual unsigned int getMask() const = 0;
		virtual bool affects(const Widget& w) const = 0;

		virtual ~WidgetEvent() = default;

	private:
		Widget* m_target;
		StopType m_stopPropagation;

	};

}