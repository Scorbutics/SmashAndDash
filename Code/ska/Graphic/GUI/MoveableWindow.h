#pragma once
#include "WindowIG.h"

namespace ska {

	class MoveableWindow;
	class ButtonWindowMover : public Button {
	public:
		ButtonWindowMover(MoveableWindow& window, Point<int> relativePos, const std::string& placeHolderStyleName, ClickEventHandler const& callback);
		~ButtonWindowMover() = default;
		virtual bool isAffectedBy(const ska::HoverEvent& e) const override;
	private:
		MoveableWindow& m_window;
	};

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