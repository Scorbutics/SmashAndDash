#include <iostream>
#include <fstream>
#include <string>

#include "Label.h"

ska::Label::Label(Widget& parent, const std::string& text, int fontSize, ska::Point<int> relativePos) : 
	Widget(parent, relativePos) {

    m_text = text;
    m_fontSize = fontSize;

    m_color.r = 0;
    m_color.g = 0;
    m_color.b = 0;
	m_color.a = 255;

    m_stext.loadFromText(m_fontSize, m_text, m_color);
	setWidth(m_stext.getWidth());
	setHeight(m_stext.getHeight());
}

void ska::Label::modifyText(const std::string& text) {
	m_text = text;
	m_stext.loadFromText(m_fontSize, m_text, m_color);
}

void ska::Label::display() const {
	const ska::Point<int>& pos = getAbsolutePosition();
	m_stext.render(pos.x, pos.y);

}

