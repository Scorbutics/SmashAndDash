#include "Widget.h"
#include "KeyEvent.h"

ska::KeyEvent::KeyEvent(const KeyEventType& state, const std::wstring& text, const int scanCode) :
	m_text(text),
	m_state(state),
	m_scanCode(scanCode) {

}

const ska::KeyEventType& ska::KeyEvent::getState() const {
	return m_state;
}

const std::wstring& ska::KeyEvent::getText() const {
	return m_text;
}

const int ska::KeyEvent::getScanCode() const {
	return m_scanCode;
}

bool ska::KeyEvent::affects(const Widget& w) const {
	return !stopped() && w.isAffectedBy(*this);
}

