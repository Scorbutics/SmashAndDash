#include <iostream>
#include <fstream>
#include <string>

#include "Image.h"

ska::Image::Image(Widget& parent, const std::string& imagePath, ska::Point<int> relativePos, bool alpha) :
	Widget(parent, relativePos),
	m_img(imagePath, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, alpha ? 128 : -1) {
	setWidth(m_img.getWidth());
	setHeight(m_img.getHeight());
}

void ska::Image::display() const {
	const ska::Point<int>& pos = getAbsolutePosition();
	m_img.render(pos.x, pos.y);

}

