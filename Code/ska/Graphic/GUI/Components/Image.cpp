#include <iostream>
#include <fstream>
#include <string>

#include "Image.h"

ska::Image::Image(Widget& parent, const std::string& imagePath, ska::Point<int> relativePos, bool alpha, ska::Rectangle* clip) :
	Widget(parent, relativePos),
	m_img(imagePath, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, alpha ? 128 : -1) {
	if(clip == nullptr) {
		m_clip.x = 0;
		m_clip.y = 0;
		m_clip.w = std::numeric_limits<int>::max();
		m_clip.h = std::numeric_limits<int>::max();
	} else {
		m_clip = *clip;
	}
	
	if (m_clip.h == std::numeric_limits<int>::max()) {
		m_clip.h = m_img.getHeight();
	}
	if (m_clip.w == std::numeric_limits<int>::max()) {
		m_clip.w = m_img.getWidth();
	}

	/* It's the Widget one which is called : polymorphism doesn't work in constructor */
	setWidth(m_clip.w);
	setHeight(m_clip.h);
}

void ska::Image::replaceWith(const std::string& imagePath, const unsigned int partsOfWidth, const unsigned int partsOfHeight, unsigned int xBlock, unsigned int yBlock) {
	m_img.load(imagePath);

	m_clip.w = m_img.getWidth() / partsOfWidth;
	m_clip.h = m_img.getHeight() / partsOfHeight;
	m_clip.x = xBlock * m_clip.w;
	m_clip.y = yBlock * m_clip.h;

	setWidth(m_clip.w);
	setHeight(m_clip.h);
}

void ska::Image::setWidth(unsigned int w) {
	ska::Widget::setWidth(w);
	if (m_clip.w == std::numeric_limits<int>::max()) {
		m_clip.w = m_img.getWidth();
	}
}

void ska::Image::setHeight(unsigned int h) {
	ska::Widget::setHeight(h);
	if (m_clip.h == std::numeric_limits<int>::max()) {
		m_clip.h = m_img.getHeight();
	}
}

void ska::Image::display() const {
	if(!isVisible()) {
		return;
	}
	const ska::Point<int>& pos = getAbsolutePosition();
	m_img.render(pos.x, pos.y, m_clip.w == std::numeric_limits<int>::max() ? nullptr : &m_clip);
}

