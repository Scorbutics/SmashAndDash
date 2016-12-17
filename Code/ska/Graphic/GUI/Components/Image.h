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
		virtual void setWidth(unsigned int w) override;
		virtual void setHeight(unsigned int h) override;
		void display() const override;

	private:
		ska::Texture m_img;
		ska::Rectangle m_clip;
	};
}
