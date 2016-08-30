#pragma once
#include "./Components/Widget.h"
#include "./Components/HasWidgets.h"
//#include "./Components/ButtonQuit.h"
#include "../Texture.h"

namespace ska{

	class WindowIG : public Widget, public HasWidgets {
	public:
		WindowIG(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle);
		WindowIG(const ska::Rectangle& box, bool drawStyle);
		void display() const override;
		~WindowIG() = default;
	
	private:
		//ska::ButtonQuit m_closeButton;
		bool m_drawStyle;
		Texture m_menuTiles;
	};
}