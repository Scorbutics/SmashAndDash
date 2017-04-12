#include "GUIScrollButtonWindowIG.h"

ska::GUIScrollButtonWindowIG::GUIScrollButtonWindowIG(Widget& parent, const Rectangle& box, const std::string& styleName) :
TimeScrollableWindowIG<>(parent, box, styleName),
m_realPos(0, 0) {

}

ska::WorkNode<ska::TimeScrollableWindowIG<>>& ska::GUIScrollButtonWindowIG::scrollTo(const Point<int>& targetPos, unsigned int speed) {
	if (!isMoving()) {
		m_realPos = getRelativePosition();
	}
	scrollStop();
	return TimeScrollableWindowIG::scrollTo(targetPos, speed);
}

ska::WorkNode<ska::TimeScrollableWindowIG<>>& ska::GUIScrollButtonWindowIG::scrollRewind() {
	scrollStop();
	return TimeScrollableWindowIG::scrollTo(m_realPos, 5);
}

ska::GUIScrollButtonWindowIG::~GUIScrollButtonWindowIG() {

}


