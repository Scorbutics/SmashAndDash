#include <iostream>

#include "Label.h"

ska::Label::Label(Widget& parent, const std::string& text, int fontSize, Point<int> relativePos) : 
	Widget(parent, relativePos), 
	m_text(text),
	m_fontSize(fontSize),
	m_clip(nullptr) {

    m_color.r = 0;
    m_color.g = 0;
    m_color.b = 0;
	m_color.a = 255;

    m_stext.loadFromText(m_fontSize, m_text, m_color);
	Widget::setWidth(m_stext.getWidth());
	Widget::setHeight(m_stext.getHeight());
}

void ska::Label::setFontColor(int r, int g, int b, int a) {
	m_color.r = static_cast<uint8_t>(r);
	m_color.g = static_cast<uint8_t>(g);
	m_color.b = static_cast<uint8_t>(b);
	m_color.a = static_cast<uint8_t>(a);
	m_stext.loadFromText(m_fontSize, m_text, m_color);
}

void ska::Label::modifyText(const std::string& text) {
	m_text = text;
	m_stext.loadFromText(m_fontSize, m_text, m_color);
	setWidth(m_stext.getWidth());
	setHeight(m_stext.getHeight());
}

void ska::Label::setClip(Rectangle* r) {
	m_clip = r;
}

void ska::Label::display() const {
	if (!isVisible()) {
		return;
	}

	const auto& pos = getAbsolutePosition();
	m_stext.render(pos.x, pos.y, m_clip);
}

void ska::Label::clear() {
	m_stext.free();
	m_text = " ";
}

