#pragma once
#include "WidgetEvent.h"


namespace ska {
	class TimeEventListener;

	class TimeEvent : public WidgetEvent<TimeEvent, TimeEventListener> {
		public:
			explicit TimeEvent(unsigned int ellapsedTime);

			virtual unsigned int getMask() const override;
			virtual bool affects(const Widget& w) const override;
			virtual StopType stopped() const override;

			unsigned int getEllapsedTime() const;

			virtual ~TimeEvent() = default;		
	private:
		const unsigned int m_ellapsedTime;
	};
}
