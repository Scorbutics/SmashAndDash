#pragma once

#include "Label.h"
#include "Button.h"
#include "../WidgetPanel.h"
#include "../../Events/KeyEventListener.h"
#include "../../Events/FocusEventListener.h"

namespace ska {
	class Input : public WidgetPanel<ValueChangedEventListener<std::wstring>, ClickEventListener, KeyEventListener, FocusEventListener> {
	public:
		Input(Widget& parent, const std::string& text, int fontSize, Point<int> relativePos);
		virtual ~Input() = default;

	private:
		void adaptDisplayWithText(Label& l);

		Rectangle m_clip;
		bool m_keyFocus;
		std::wstring m_rawText;
		std::wstring m_lastRawText;
	};
}
