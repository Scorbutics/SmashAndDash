#include <iostream>
#include <string>

#include "Window_Area.h"
#include "DialogMenu.h"

Window_Area::Window_Area(DialogMenu *parent) {
    m_active = false;
    m_parent = parent;
    m_relativePos.x = 0;
    m_relativePos.y = 0;
}

ska::Point<int>& Window_Area::getRelativePos() {
    return m_relativePos;
}

DialogMenu* Window_Area::getParent() {
    return m_parent;
}

ska::Point<int> Window_Area::getAbsolutePos()
{
	ska::Point<int> buf = m_relativePos;
    buf.x += (m_parent->getRect()).x;
	buf.y += (m_parent->getRect()).y;
    return buf;
}

bool Window_Area::isVisible()
{
    return m_active;
}

void Window_Area::hide(bool x)
{
    m_active = !x;
}

bool Window_Area::isA(int type)
{
    return (m_type == type);
}

void Window_Area::setRelativePos(ska::Point<int> pos)
{
    m_relativePos.x = pos.x;
    m_relativePos.y = pos.y;
}

bool Window_Area::canBeClicked()
{
    return m_canBeClicked;
}

Window_Area::~Window_Area(){}
