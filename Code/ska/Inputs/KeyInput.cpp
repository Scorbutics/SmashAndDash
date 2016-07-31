#include <iostream>
#include <string>

#include "KeyInput.h"


ska::KeyInput::KeyInput() {
}

ska::KeyInput::~KeyInput() {
}

void ska::KeyInput::setKeyState(int key, bool state) {
	m_toggle[key] = state;
	m_keys[key] = state;
}

bool ska::KeyInput::toggle(int key) {
	return m_toggle[key];
}

bool ska::KeyInput::trigger(int key) {
	return m_keys[key];
}

void ska::KeyInput::resetTriggers() {
	m_keys.reset();
}
