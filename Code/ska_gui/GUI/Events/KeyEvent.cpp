#include "KeyEvent.h"
#include "../Utils/WidgetMaskHelper.h"

ska::KeyEvent::KeyEvent(const KeyEventType& state, const std::wstring& text, const int scanCode) :
	m_state(state),
	m_text(text),
	m_scanCode(scanCode) {

}

const ska::KeyEventType& ska::KeyEvent::getState() const {
	return m_state;
}

const std::wstring& ska::KeyEvent::getText() const {
	return m_text;
}

int ska::KeyEvent::getScanCode() const {
	return m_scanCode;
}

bool ska::KeyEvent::affects(const Widget&) const {
	return stopped() == NOT_STOPPED;
}

unsigned int ska::KeyEvent::getMask() const {
	return WidgetMaskHelper::template getMask<KeyEventListener>();
}
