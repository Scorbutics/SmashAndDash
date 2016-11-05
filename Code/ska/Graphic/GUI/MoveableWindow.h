#pragma once
#include "WindowIG.h"

namespace ska {

	class MoveableWindow : public WindowIG {
		friend class ButtonWindowMover;
	public:
		MoveableWindow(MouseObservable& guiObservable, const ska::Rectangle& box, bool drawStyle);
		MoveableWindow(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle);

	private:
		void initHandlers();
		bool m_moving;
		ska::Point<int> m_offsetWindowOrigin;
	};
}