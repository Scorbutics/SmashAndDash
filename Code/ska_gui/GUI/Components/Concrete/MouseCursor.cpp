#include <SDL.h>
#include "MouseCursor.h"

ska::MouseCursor::MouseCursor(const std::string& styleName) :
	m_basicCursor(styleName + ".png"),
	m_grabCursor(styleName + "_grab.png"),
	m_currentCursor(&m_basicCursor),
	m_visible(false) {
	SDL_ShowCursor(false);
}

void ska::MouseCursor::move(const Point<int>& mousePos) {
	m_mousePos = mousePos;
}

bool ska::MouseCursor::isVisible() const{
	return m_visible;
}

void ska::MouseCursor::grab(){
	m_currentCursor = &m_grabCursor;
}

void ska::MouseCursor::basic(){
	m_currentCursor = &m_basicCursor;
}

ska::MouseCursor::~MouseCursor(){}

void ska::MouseCursor::display() const{
	if (m_currentCursor != nullptr) {
		m_currentCursor->render(m_mousePos.x, m_mousePos.y);
	}
}
