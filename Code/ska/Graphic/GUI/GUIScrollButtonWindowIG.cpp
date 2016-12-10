#include "GUIScrollButtonWindowIG.h"

ska::GUIScrollButtonWindowIG::GUIScrollButtonWindowIG(ska::Widget& parent, const ska::Rectangle& box, const std::string& styleName) :
DynamicWindowIG<>(parent, box, styleName),
m_realPos(0, 0) {

}

bool ska::GUIScrollButtonWindowIG::scrollTo(const ska::Point<int>& targetPos, unsigned int speed) {
	if (!isMoving()) {
		m_realPos = getRelativePosition();
	}
	scrollStop();
	return TimeScrollableWindowIG::scrollTo(targetPos, speed);
}

bool ska::GUIScrollButtonWindowIG::scrollRewind() {
	scrollStop();
	return TimeScrollableWindowIG::scrollTo(m_realPos, 5);
}

ska::GUIScrollButtonWindowIG::~GUIScrollButtonWindowIG() {

}


