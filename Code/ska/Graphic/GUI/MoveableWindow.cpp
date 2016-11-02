#include <iostream>
#include "MoveableWindow.h"
#include "Components/Button.h"

ska::MoveableWindow::MoveableWindow(MouseObservable& guiObservable, const ska::Rectangle& box, bool drawStyle) : 
ska::WindowIG(guiObservable, box, drawStyle) {
	initHandlers();
}

void ska::MoveableWindow::initHandlers() {
	auto button = std::make_unique<ska::ButtonWindowMover>(*this, ska::Point<int>(), "", [&](ska::Widget* tthis, ska::ClickEvent& e) {
		if (e.getState() == ska::MouseEventType::MOUSE_CLICK) {
			m_moving = true;
			m_offsetWindowOrigin = e.getPosition(*tthis);
		}
		else if (e.getState() == ska::MouseEventType::MOUSE_RELEASE) {
			m_moving = false;
		} else {
			return false;
		}
		return true;
	});
	button->setWidth(getBox().w);
	button->setHeight(TAILLEBLOCFENETRE / 2);
	button->addHoverHandler([&](ska::Widget* tthis, ska::HoverEvent& e) {
		if (m_moving && e.getState() != ska::MouseEventType::MOUSE_ENTER) {
			const auto& clickAbsPos = e.getMousePosition();
			const auto& newPos = clickAbsPos - m_offsetWindowOrigin;
			move(newPos);
		}
		return false;
	});
	addWidget(std::move(button));
}

ska::MoveableWindow::MoveableWindow(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle) :
ska::WindowIG(parent, box, drawStyle) {
	initHandlers();
}

ska::ButtonWindowMover::ButtonWindowMover(MoveableWindow& window, Point<int> relativePos, const std::string& placeHolderStyleName, ClickEventHandler const& callback) : 
	ska::Button(window, relativePos, placeHolderStyleName, callback),
	m_window(window) {
	
}

bool ska::ButtonWindowMover::isAffectedBy(const ska::HoverEvent& e) const {
// 	if(e.getState() == ska::MouseEventType::MOUSE_OVER || e.getState() == ska::MouseEventType::MOUSE_RELEASE) {
// 		return true;
// 	}

	return ska::Widget::isAffectedBy(e);
}
