#pragma once
#include "../Widget.h"
#include "Graphic/Texture.h"


namespace ska {
	class Image : public Widget {
	public:
		Image(Widget& parent, const std::string& imagePath, Point<int> relativePos, bool alpha, Rectangle* clip);
		virtual ~Image() = default;
		void replaceWith(const std::string& imagePath, const unsigned int partsOfWidth, const unsigned int partsOfHeight, unsigned int xBlock, unsigned int yBlock);
		virtual void setWidth(unsigned int w) override;
		virtual void setHeight(unsigned int h) override;
		void display() const override;

	private:
		Texture m_img;
		Rectangle m_clip;
	};
}
