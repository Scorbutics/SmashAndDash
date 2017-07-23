#include "WindowInput.h"

ska::WindowInput::WindowInput() : 
	m_width(0), 
	m_height(0) {
}

void ska::WindowInput::setSize(Sint32 data1, Sint32 data2) {
	m_width = data1;
	m_height = data2;
}

unsigned int ska::WindowInput::getWidth() const {
	return m_width;
}

unsigned int ska::WindowInput::getHeight() const {
	return m_height;
}
