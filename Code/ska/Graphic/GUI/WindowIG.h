#pragma once
#include "./Components/Button.h"
#include "./Components/HasWidgets.h"
#include "./Components/MouseObserver.h"
#include "./Components/MouseObservable.h"
#include "./Components/KeyboardObservable.h"
#include "../Texture.h"

namespace ska{
	
	class WindowIG : 
		public Button,
		public HasWidgets {
	public:
		WindowIG(MouseObservable& guiObservable, KeyboardObservable& keyObservable, const ska::Rectangle& box, bool drawStyle);
		WindowIG(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle);
		virtual bool isAffectedBy(const ska::KeyEvent& e) const;
		void display() const override;
		virtual ~WindowIG();
	
	private:
		void initHandlers();

		bool m_drawStyle;
		Texture m_menuTiles;
		MouseObservable *const m_guiObservable;
		KeyboardObservable *const m_keyObservable;
	};
}