#include "GUIScrollButtonWindowIG.h"

ska::GUIScrollButtonWindowIG::GUIScrollButtonWindowIG(MouseObservable& guiObservable, const ska::Rectangle& box, bool drawStyle) : 
	TimeScrollableWindowIG(guiObservable, box, drawStyle), 
	m_realPos(getRelativePosition()){
	
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

ska::GUIScrollButtonWindowIG::GUIScrollButtonWindowIG(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle) :
	TimeScrollableWindowIG(parent, box, drawStyle),
	m_realPos(0, 0) {

}

