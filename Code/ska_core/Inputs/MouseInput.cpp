#include <cstring>
#include "MouseInput.h"

ska::MouseInput::MouseInput() {
	resetAll();
}


ska::MouseInput::~MouseInput() {
}


const ska::Point<int>& ska::MouseInput::getMouseLastPos() const {
	return m_mouseLastPos;
}

void ska::MouseInput::setMouseState(int touche, int state) {
	m_mouseToggle[touche] = state;

	m_mouseState[touche] = state;
	if (mouseClick(touche)) {
		m_clickPos = m_mouse;
	}

}

int ska::MouseInput::trigger(int touche) const {
	return m_mouseState[touche];
}

int ska::MouseInput::toggle(int touche) const {
	return m_mouseToggle[touche];
}


void ska::MouseInput::setMousePos(SDL_Event event) {
	m_mouse = std::move(Point<int>(event.motion.x, event.motion.y));
	m_mouseRel = std::move(Point<int>(event.motion.xrel, event.motion.yrel));
}

const ska::Point<int>& ska::MouseInput::getMousePos() const {
	return m_mouse;
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

const ska::Point<int>& ska::MouseInput::getMouseClickPos() const {
	return m_clickPos;
}

ska::Point<int> ska::MouseInput::getMouseTranslation() const {
	return std::move(Point<int>(m_mouse.x - m_mouseLastPos.x, m_mouse.y - m_mouseLastPos.y));
}

void ska::MouseInput::setMouseLastPos(const Point<int>& mouselastpos) {
	m_mouseLastPos.x = mouselastpos.x;
	m_mouseLastPos.y = mouselastpos.y;
}
