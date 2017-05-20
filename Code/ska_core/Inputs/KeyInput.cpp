#include <iostream>
#include <string>

#include "KeyInput.h"


void ska::KeyInput::setKeyState(int key, bool state) {
	m_toggle[key] = state;
	m_keys[key] = state;
}

bool ska::KeyInput::toggle(int key) const {
	return m_toggle[key];
}

bool ska::KeyInput::trigger(int key) const {
	return m_keys[key];
}

void ska::KeyInput::resetTriggers() {
	m_keys.reset();
}
