#include <cstring>
#include "MouseInput.h"

ska::MouseInput::MouseInput() {
	m_mousex = 0;
	m_mousey = 0;
	m_mousexrel = 0;
	m_mouseyrel = 0;
	m_clickPos.x = m_clickPos.y = 0;
	resetAll();
}


ska::MouseInput::~MouseInput() {
}


ska::Point<int> ska::MouseInput::getMouseLastPos() const {
	return m_mouseLastPos;
}

void ska::MouseInput::setMouseState(int touche, int state) {	
	m_mouseToggle[touche] = state;
		
	m_mouseState[touche] = state;
	if (mouseClick(touche)) {
		m_clickPos.x = m_mousex;
		m_clickPos.y = m_mousey;
	}

}

int ska::MouseInput::trigger(int touche) const {
	return m_mouseState[touche];
}

int ska::MouseInput::toggle(int touche) const {
	return m_mouseToggle[touche];
}


void ska::MouseInput::setMousePos(SDL_Event event) {
	m_mousex = event.motion.x;
	m_mousey = event.motion.y;
	m_mousexrel = event.motion.xrel;
	m_mouseyrel = event.motion.yrel;
}

ska::Point<int> ska::MouseInput::getMousePos() const {
	Point<int> buf;
	buf.x = m_mousex;
	buf.y = m_mousey;
	return buf;
}

void ska::MouseInput::resetAll() {
	memset(m_mouseToggle, 0, sizeof(m_mouseToggle));
	resetTriggers();
}

void ska::MouseInput::resetTriggers() {
	memset(m_mouseState, 0, sizeof(m_mouseState));
}

bool ska::MouseInput::mouseClick(int touche) {
	if (m_mouseState[touche]) {
		return true;
	}

	return false;
}

ska::Point<int> ska::MouseInput::getMouseClickPos() const {
	return m_clickPos;
}

ska::Point<int> ska::MouseInput::getMouseTranslation() const {
	Point<int> mouseTrans;
	mouseTrans.x = m_mousex - m_mouseLastPos.x;
	mouseTrans.y = m_mousey - m_mouseLastPos.y;
	return mouseTrans;
}

void ska::MouseInput::setMouseLastPos(Point<int> mouselastpos) {
	m_mouseLastPos = mouselastpos;
}
