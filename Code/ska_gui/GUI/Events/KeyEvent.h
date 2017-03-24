#pragma once
#include "WidgetEvent.h"
#include "KeyEventListener.h"

namespace ska {
	enum KeyEventType {
		KEY_FOCUS,
		KEY_BLUR,
		TEXT_TYPED,
		KEY_DOWN,
		KEY_UP
	};
	class Widget;
	class KeyEvent : public WidgetEvent<KeyEvent, KeyEventListener> {
	public:
		KeyEvent(const KeyEventType& state, const std::wstring& text, const int scanCode);

		void operator=(const KeyEvent&) = delete;

		const KeyEventType& getState() const;
		const std::wstring& getText() const;
		int getScanCode() const;

		virtual bool affects(const Widget& w) const override;
		virtual unsigned int getMask() const override;

		virtual ~KeyEvent() = default;

	private:
		const KeyEventType m_state;
		const std::wstring m_text;
		const int m_scanCode;

	};
}
