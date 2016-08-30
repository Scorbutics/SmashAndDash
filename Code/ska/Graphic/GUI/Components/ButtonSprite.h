#pragma once

#include "Button.h"

namespace ska {
	class ClickEvent;

	class ButtonSprite : 
		public Button {

	public:
		ButtonSprite(GUI& guiObservable, Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, unsigned int id, ClickEventHandler const& callback);
		virtual ~ButtonSprite() = default;

		virtual void display() const override;
		virtual void move(const ska::Point<int>& pos) override;

	private:
		ska::Texture m_img;
		ska::Rectangle m_rect;
	};
}
