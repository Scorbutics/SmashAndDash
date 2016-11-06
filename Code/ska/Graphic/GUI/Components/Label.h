#pragma once

#include <string>
#include "../../Texture.h"
#include "../../Color.h"
#include "Widget.h"

namespace ska {
	class Label : public Widget {
	public:
		Label(Widget& parent, const std::string& text, int fontSize, ska::Point<int> relativePos);
		virtual ~Label() = default;

		void modifyText(const std::string& text);
		void display() const override;

	private:
		std::string m_text;
		unsigned int m_fontSize;
		ska::Texture m_stext;
		ska::Color m_color;
	};
}
