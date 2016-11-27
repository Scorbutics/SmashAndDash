#pragma once

#include <string>
#include "Label.h"
#include "Button.h"

#include "ClickEventListener.h"
#include "KeyEventListener.h"
#include "FocusEventListener.h"

namespace ska {
	class Input : public HandledWidget<ClickEventListener, KeyEventListener, FocusEventListener> {
	public:
		Input(Widget& parent, const std::string& text, int fontSize, ska::Point<int> relativePos);
		virtual ~Input() = default;
		void display() const override;

	private:
		Button m_field;
		Label m_text;
		bool m_keyFocus;
		std::wstring m_rawText;
	};
}
