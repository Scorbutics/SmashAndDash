#include <iostream>
#include "MoveableWindow.h"
#include "Components/Button.h"
#include "Components/ButtonQuit.h"

ska::MoveableWindow::MoveableWindow(MouseObservable& guiObservable, const ska::Rectangle& box, bool drawStyle) : 
ska::WindowIG(guiObservable, box, drawStyle) {
	initHandlers();
}

ska::MoveableWindow::MoveableWindow(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle) :
ska::WindowIG(parent, box, drawStyle) {
	initHandlers();
}

void ska::MoveableWindow::initHandlers() {
	const auto& clip = ska::Rectangle{ 0, 0, getBox().w, TAILLEBLOCFENETRE / 2 };
	auto button = std::make_unique<ska::Button>(*this, ska::Point<int>(), ska::Button::MENU_DEFAULT_THEME_PATH + "button", &clip, [&](ska::Widget* tthis, ska::ClickEvent& e) {
		if (e.getState() == ska::MouseEventType::MOUSE_CLICK) {
			m_moving = true;
			m_offsetWindowOrigin = e.getPosition(*tthis);
		}
		else if (e.getState() == ska::MouseEventType::MOUSE_RELEASE) {
			m_moving = false;
		}
	});
	button->setWidth(getBox().w - TAILLEBLOCFENETRE / 2);
	button->setHeight(TAILLEBLOCFENETRE / 2);
	button->addHoverHandler([&](ska::Widget* tthis, ska::HoverEvent& e) {
		if (m_moving && e.getState() != ska::MouseEventType::MOUSE_ENTER) {
			const auto& clickAbsPos = e.getMousePosition();
			const auto& newPos = clickAbsPos - m_offsetWindowOrigin;
			move(newPos);
		}		
	});

	auto buttonQuit = std::make_unique<ska::ButtonQuit>(*this, ska::Point<int>(getBox().w - TAILLEBLOCFENETRE/2, 0), ska::Button::MENU_DEFAULT_THEME_PATH + "close_button");

	addWidget(std::move(button));
	addWidget(std::move(buttonQuit));
}

