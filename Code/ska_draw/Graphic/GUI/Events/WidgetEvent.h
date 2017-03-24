#pragma once
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

		void stopPropagation(const StopType& st) override{
			m_stopPropagation = st;
		}

		StopType stopped() const override{
			return m_stopPropagation;
		}

		Widget* getTarget() const{
			return m_target;
		}

		Widget const* getCTarget() const {
			return m_target;
		}

		virtual unsigned int getMask() const override = 0;
		virtual bool affects(const Widget& w) const override = 0;

		virtual ~WidgetEvent() = default;

	private:
		Widget* m_target;
		StopType m_stopPropagation;

	};

}
