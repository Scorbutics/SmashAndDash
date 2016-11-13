#pragma once
#include "EventListener.h"

namespace ska {
	class Widget;

	enum StopType {
		NOT_STOPPED,
		STOP_CALLBACK,
		STOP_WIDGET
	};

	class IWidgetEvent {
	public:

		virtual bool affects(const Widget& w) const = 0;
		virtual unsigned int getMask() const = 0;
		virtual StopType stopped() const = 0;
		virtual void stopPropagation(const StopType& st) = 0;
	};
}