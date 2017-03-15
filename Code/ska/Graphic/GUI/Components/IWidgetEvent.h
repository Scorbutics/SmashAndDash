#pragma once
#include "EventListener.h"
#include "StopType.h"
namespace ska {
	class Widget;

	class IWidgetEvent {
	public:
		virtual ~IWidgetEvent(){}

		virtual bool affects(const Widget& w) const = 0;
		virtual unsigned int getMask() const = 0;
		virtual StopType stopped() const = 0;
		virtual void stopPropagation(const StopType& st) = 0;
	};
}
