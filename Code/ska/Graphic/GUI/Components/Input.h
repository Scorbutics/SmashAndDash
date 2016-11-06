#pragma once

#include <string>
#include "Label.h"
#include "Button.h"

namespace ska {
	class Input : public Widget {
	public:
		Input(Widget& parent, const std::string& text, int fontSize, ska::Point<int> relativePos);
		virtual ~Input() = default;
		virtual bool isAffectedBy(const ska::KeyEvent& e) const;
		void display() const override;

	private:
		Button m_field;
		Label m_text;
		bool m_keyFocus;
		std::wstring m_rawText;
	};
}
