#include <iostream>
#include <fstream>
#include <string>

#include "Label.h"

ska::Label::Label(Widget& parent, const std::string& text, int fontSize, ska::Point<int> relativePos) : 
	Widget(parent, relativePos), 
	m_fontSize(fontSize),
	m_text(text) {

    m_color.r = 0;
    m_color.g = 0;
    m_color.b = 0;
	m_color.a = 255;

    m_stext.loadFromText(m_fontSize, m_text, m_color);
	setWidth(m_stext.getWidth());
	setHeight(m_stext.getHeight());
}

void ska::Label::setFontColor(int r, int g, int b, int a) {
	m_color.r = r;
	m_color.g = g;
	m_color.b = b;
	m_color.a = a;
	m_stext.loadFromText(m_fontSize, m_text, m_color);
}

void ska::Label::modifyText(const std::string& text) {
	m_text = text;
	m_stext.loadFromText(m_fontSize, m_text, m_color);
	setWidth(m_stext.getWidth());
	setHeight(m_stext.getHeight());
}

void ska::Label::setClip(ska::Rectangle* r) {
	m_clip = r;
}

void ska::Label::display() const {
	if (!isVisible()) {
		return;
	}

	const ska::Point<int>& pos = getAbsolutePosition();
	m_stext.render(pos.x, pos.y, m_clip);
}

