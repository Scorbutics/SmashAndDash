#pragma once

#include <string>
#include "Label.h"
#include "Button.h"
#include "WidgetPanel.h"

#include "ClickEventListener.h"
#include "KeyEventListener.h"
#include "FocusEventListener.h"
#include "ValueChangedEvent.h"
#include "ValueChangedEventListener.h"

namespace ska {
	class Input : public WidgetPanel<ValueChangedEventListener<std::wstring>, ClickEventListener, KeyEventListener, FocusEventListener> {
	public:
		Input(Widget& parent, const std::string& text, int fontSize, ska::Point<int> relativePos);
		virtual ~Input() = default;

	private:
		void adaptDisplayWithText(Label& l);

		ska::Rectangle m_clip;
		bool m_keyFocus;
		std::wstring m_rawText;
	};
}
