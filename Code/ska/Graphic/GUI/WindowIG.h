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
		virtual ~WindowIG();
	
	private:
		void initHandlers();

		bool m_drawStyle;
		Texture m_menuTiles;
		MouseObservable *const m_guiObservable;
	};
}