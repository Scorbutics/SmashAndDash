#pragma once
#include "./Components/Button.h"
#include "./Components/HasWidgets.h"
#include "./Components/MouseObserver.h"
#include "./Components/MouseObservable.h"
#include "../Texture.h"

namespace ska{
	
	class WindowIG : 
		public Button,
		public HasWidgets {
	public:
		WindowIG(MouseObservable& guiObservable, const ska::Rectangle& box, bool drawStyle);
		WindowIG(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle);
		void display() const override;
		bool onClick(ska::ClickEvent& e);
		bool onMouseHover(ska::HoverEvent& e);
		void refresh();
		void scrollTo(const ska::Point<int>& targetPos, unsigned int speed);
		~WindowIG();
	
	private:
		//ska::ButtonQuit m_closeButton;
		bool m_drawStyle;
		Texture m_menuTiles;
		MouseObservable *const m_guiObservable;
		ska::Point<int> m_destinationPos;
		ska::Point<double> m_slope;
		double m_speed;
	};
}