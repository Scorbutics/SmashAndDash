#pragma once
#include "./Components/Widget.h"
#include "./Components/HasWidgets.h"
#include "./Components/MouseObserver.h"
#include "./Components/MouseObservable.h"
#include "../Texture.h"

namespace ska{
	
	class WindowIG : 
		public Widget,
		public HasWidgets,
		public MouseObserver,
		public MouseObservable {
	public:
		WindowIG(MouseObservable& guiObservable, ska::Widget& parent, const ska::Rectangle& box, bool drawStyle);
		WindowIG(MouseObservable& guiObservable, const ska::Rectangle& box, bool drawStyle);
		void display() const override;
		bool click(ska::ClickEvent& e) override;
		bool mouseHover(ska::HoverEvent& e) override;
		~WindowIG();
	
	private:
		//ska::ButtonQuit m_closeButton;
		bool m_drawStyle;
		Texture m_menuTiles;
		MouseObservable& m_guiObservable;
	};
}