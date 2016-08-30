#include "Widget.h"

ska::Widget::Widget() :
	m_parent(nullptr) {
}

ska::Widget::Widget(Widget& parent) :
	m_parent(&parent){
	m_box.x = 0;
	m_box.y = 0;
	m_box.w = 0;
	m_box.h = 0;
}

ska::Widget::Widget(Widget& parent, Point<int>& position) : 
	m_parent(&parent) {
	m_box.x = position.x;
	m_box.y = position.y;
	m_box.w = 0;
	m_box.h = 0;
}

void ska::Widget::setWidth(unsigned int w) {
	m_box.w = w;
}

void ska::Widget::setHeight(unsigned int h) {
	m_box.h = h;
}

const ska::Point<int> ska::Widget::getAbsolutePosition() const {
	if (m_parent == nullptr) {
		return m_box;
	}

	return m_parent->getAbsolutePosition() + ska::Point<int>(m_box);
}

const ska::Point<int> ska::Widget::getRelativePosition() const {
	return m_box;
}

void ska::Widget::show(bool sh) {
	m_visible = sh;
}

void ska::Widget::move(const ska::Point<int>& pos) {
	m_box.x = pos.x;
	m_box.y = pos.y;
}

bool ska::Widget::isVisible() const {
	return m_visible;
}

const ska::Rectangle& ska::Widget::getBox() const {
	return m_box;
}

