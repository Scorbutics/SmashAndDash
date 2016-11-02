#pragma once
#include "WindowIG.h"


namespace ska {
	class TimeScrollableWindowIG : public WindowIG {
	public:
		TimeScrollableWindowIG(MouseObservable& guiObservable, const ska::Rectangle& box, bool drawStyle);
		TimeScrollableWindowIG(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle);
		bool refresh();
		virtual bool scrollTo(const ska::Point<int>& targetPos, unsigned int steps);
		void scrollStop();
		virtual ~TimeScrollableWindowIG() = default;
	
	protected:
		bool isMoving() const;

	private:
		Point<int> m_destinationPos;
		ska::Point<double> m_slope;
		bool m_moving;

	};
}