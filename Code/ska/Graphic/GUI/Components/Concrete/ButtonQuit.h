#pragma once

#include "Button.h"

namespace ska {
	class ClickEvent;

	class ButtonQuit : 
		public Button {

	public:
		ButtonQuit(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName);
		virtual ~ButtonQuit() = default;
	};
}
