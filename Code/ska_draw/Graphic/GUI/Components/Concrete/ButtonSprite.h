#pragma once

#include "Button.h"

namespace ska {
	class ClickEvent;

	class ButtonSprite : 
		public Button {

	public:
		ButtonSprite(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, unsigned int id, ClickEventHandler const& callback);
		ButtonSprite(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const std::string& imagePath, const Rectangle& clip, ClickEventHandler const& callback);
		virtual ~ButtonSprite() = default;

		void replaceWith(const std::string& path, const Rectangle& clip);
		void clear();

		virtual void display() const override;
		virtual void move(const Point<int>& pos) override;

	private:
		Texture m_img;
		Rectangle m_rect;
		Rectangle m_clip;
	};
}
