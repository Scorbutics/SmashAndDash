#pragma once
#include "WidgetEvent.h"


namespace ska {
	class TimeEventListener;

	class TimeEvent : public WidgetEvent<TimeEvent, TimeEventListener> {
		public:
			TimeEvent() = default;

			virtual unsigned int getMask() const override;
			virtual bool affects(const Widget& w) const override;
			virtual StopType stopped() const override;

			virtual ~TimeEvent() = default;		
	};
}
