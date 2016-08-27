#pragma once

#include <string>
#include "../../Texture.h"
#include "../../Color.h"
#include "Widget.h"

namespace ska {
	class Image : public Widget {
	public:
		Image(Widget& parent, const std::string& imagePath, ska::Point<int> relativePos, bool alpha);
		virtual ~Image() = default;
		void display() const override;

	private:
		ska::Texture m_img;
	};
}
