#pragma once

#include "Button.h"

namespace ska {
	class ClickEvent;

	class ButtonSprite : 
		public Button {

	public:
		ButtonSprite(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, unsigned int id, ClickEventHandler const& callback);
		ButtonSprite(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const std::string& imagePath, ClickEventHandler const& callback);
		virtual ~ButtonSprite() = default;

		virtual void display() const override;
		virtual void move(const ska::Point<int>& pos) override;

	private:
		ska::Texture m_img;
		ska::Rectangle m_rect;
	};
}
