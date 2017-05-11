#include "Widget.h"

ska::Widget::Widget() : m_parent(nullptr),
m_visible(true),
m_focused(false) {
	m_box.x = 0;
	m_box.y = 0;
	m_box.w = 1;
	m_box.h = 1;

}

ska::Widget::Widget(Widget& parent) :
m_parent(&parent),
m_visible(true),
m_focused(false) {
	m_box.x = 0;
	m_box.y = 0;
	m_box.w = 1;
	m_box.h = 1;
}

ska::Widget::Widget(Widget& parent, Point<int>& position) :
m_parent(&parent),
m_visible(true),
m_focused(false) {
	m_box.x = position.x;
	m_box.y = position.y;
	m_box.w = 1;
	m_box.h = 1;

}

bool ska::Widget::isVisible() const {
	return m_visible;
}

void ska::Widget::setWidth(unsigned int w) {
	m_box.w = w;
}

void ska::Widget::setHeight(unsigned int h) {
	m_box.h = h;
}

void ska::Widget::show(bool sh) {
	m_visible = sh;
}

void ska::Widget::move(const Point<int>& pos) {
	m_box.x = pos.x;
	m_box.y = pos.y;
}

ska::Widget* ska::Widget::getParent() const {
	return m_parent;
}

bool ska::Widget::isAParent(const Widget& w) const {
	if (this == &w) {
		return true;
	}

	return w.getParent() == nullptr ? false : isAParent(*w.getParent());
}

/* DEBUG ONLY */
void ska::Widget::setName(const std::string& s) {
	m_name = s;
}

const std::string& ska::Widget::getName() const {
	return m_name;
}
/* END DEBUG ONLY */

const ska::Rectangle& ska::Widget::getBox() const {
	return m_box;
}

bool ska::Widget::notify(IWidgetEvent&) {
	/* A non-handled widget cannot be notified */
	return false;
}

ska::Point<int> ska::Widget::getAbsolutePosition() const {
	if (m_parent == nullptr) {
		return m_box;
	}

	return m_parent->getAbsolutePosition() + m_box;
}

ska::Point<int> ska::Widget::getRelativePosition() const {
	return m_box;
}

void ska::Widget::focus(bool f) {
	m_focused = f;
}

bool ska::Widget::isFocused() const {
	return m_focused;
}

