#pragma once

#include <string>
#include "../../Texture.h"
#include "../../Color.h"
#include "Widget.h"

namespace ska {
	class Image : public Widget {
	public:
		Image(Widget& parent, const std::string& imagePath, ska::Point<int> relativePos, bool alpha, ska::Rectangle* clip);
		virtual ~Image() = default;
		void replaceWith(const std::string& imagePath, const unsigned int partsOfWidth, const unsigned int partsOfHeight, unsigned int xBlock, unsigned int yBlock);
		virtual void setWidth(unsigned int w) override;
		virtual void setHeight(unsigned int h) override;
		void display() const override;

	private:
		ska::Texture m_img;
		ska::Rectangle m_clip;
	};
}
