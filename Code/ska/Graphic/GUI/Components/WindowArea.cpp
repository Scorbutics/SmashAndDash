// #include <iostream>
// #include <string>
// 
// #include "WindowArea.h"
// #include "Widget.h"
// 
// ska::WindowArea::WindowArea(Widget& parent) : m_parent(parent) {
//     m_active = false;
//     m_relativePos.x = 0;
//     m_relativePos.y = 0;
// }
// 
// ska::Point<int>& ska::WindowArea::getRelativePos() {
//     return m_relativePos;
// }
// 
// Widget& ska::WindowArea::getParent() {
//     return m_parent;
// }
// 
// ska::Point<int> ska::WindowArea::getAbsolutePos() {
// 	ska::Point<int> buf = m_relativePos;
//     buf.x += (m_parent.getRect()).x;
// 	buf.y += (m_parent.getRect()).y;
//     return buf;
// }
// 
// bool ska::WindowArea::isVisible() {
//     return m_active;
// }
// 
// void ska::WindowArea::hide(bool x) {
//     m_active = !x;
// }
// 
// void ska::WindowArea::setRelativePos(ska::Point<int> pos) {
//     m_relativePos.x = pos.x;
//     m_relativePos.y = pos.y;
// }
// 
// bool ska::WindowArea::canBeClicked() {
//     return m_canBeClicked;
// }
// 
// ska::WindowArea::~WindowArea() {
// }
