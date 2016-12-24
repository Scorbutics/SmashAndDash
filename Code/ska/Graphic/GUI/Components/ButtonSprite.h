#pragma once

#include "Button.h"

namespace ska {
	class ClickEvent;

	class ButtonSprite : 
		public Button {

	public:
		ButtonSprite(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, unsigned int id, ClickEventHandler const& callback);
		ButtonSprite(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const std::string& imagePath, const ska::Rectangle& clip, ClickEventHandler const& callback);
		virtual ~ButtonSprite() = default;

		void replaceWith(const std::string& path, const ska::Rectangle& clip);
		virtual void display() const override;
		virtual void move(const ska::Point<int>& pos) override;

	private:
		ska::Texture m_img;
		ska::Rectangle m_rect;
		ska::Rectangle m_clip;
	};
}
