#pragma once
#include <iostream>
#include "WidgetEvent.h"

namespace ska {
	enum KeyEventType {
		KEY_FOCUS,
		KEY_BLUR,
		TEXT_TYPED,
		KEY_DOWN,
		KEY_UP
	};
	class Widget;
	class KeyEvent : public WidgetEvent {
	public:
		KeyEvent(const KeyEventType& state, const std::wstring& text, const int scanCode);
		const KeyEventType& getState() const;
		const std::wstring& getText() const;
		const int getScanCode() const;

		virtual bool affects(const Widget& w) const override;
		virtual ~KeyEvent() = default;

	private:
		const KeyEventType m_state;
		const std::wstring m_text;
		const int m_scanCode;

	};
}