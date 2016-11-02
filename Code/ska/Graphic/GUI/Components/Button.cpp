#include <iostream>
#include "Button.h"
#include "ClickEvent.h"
#include "../Components/MouseObservable.h"

ska::Button::Button(Widget& parent, ska::Point<int> relativePos, const std::string& placeHolderStyleName, ClickEventHandler const& callback) :
Widget(parent),
m_state(ska::ButtonState::NONE),
m_placeHolder(placeHolderStyleName + ".png"),
m_placeHolderHover(placeHolderStyleName + "_hover.png"),
m_placeHolderPressed(placeHolderStyleName + "_pressed.png"),
m_drawStyle(!placeHolderStyleName.empty()) {
	move(getRelativePosition() + relativePos);

	m_textureSelector = &m_placeHolderHover;
	setWidth(m_placeHolder.getWidth());
	setHeight(m_placeHolder.getHeight());

	initHandlers();
	addClickHandler(callback);
}

ska::Button::Button() : 
m_state(ska::ButtonState::NONE),
m_drawStyle(false) {
	m_textureSelector = &m_placeHolder;
	initHandlers();
}

ska::Button::Button(Widget& parent) : 
Widget(parent),
m_state(ska::ButtonState::NONE),
m_drawStyle(false) {
	initHandlers();
}

void ska::Button::initHandlers() {
	addHoverHandler([this](ska::Widget* tthis, ska::HoverEvent& e) {
		bool stopEventPropagation = false;
		bool target = false;
		switch (e.getState()) {
		case ska::MouseEventType::MOUSE_ENTER:
			if (m_state != ButtonState::HOVER && m_state != ButtonState::ENTER && m_state != ButtonState::PRESSED) {
				m_state = ButtonState::ENTER;
				m_textureSelector = &m_placeHolderHover;
				target = true;
			} else {
				stopEventPropagation = true;
			}
			break;

		case ska::MouseEventType::MOUSE_OUT:
			if (m_state == ButtonState::HOVER || m_state == ButtonState::ENTER) {
				m_state = ButtonState::NONE;
				m_textureSelector = &m_placeHolder;
				target = true;
			} else {
				stopEventPropagation = true;
			}
			break;
		case ska::MouseEventType::MOUSE_OVER:
			m_state = ButtonState::HOVER;
			target = true;
			break;
		default:
			break;
		}

		if (target && e.getTarget() == nullptr) {
			e.setTarget(this);
		}

		return stopEventPropagation;
	});

	addClickHandler([this](ska::Widget* tthis, ska::ClickEvent& e) {
		bool eventPropagation = true;
		switch (e.getState()) {
		case ska::MouseEventType::MOUSE_CLICK:
			m_state = ButtonState::PRESSED;
			//m_textureSelector = &m_placeHolderPressed;
			eventPropagation = false;
			break;
		case ska::MouseEventType::MOUSE_RELEASE:
			m_state = ButtonState::HOVER;
			eventPropagation = false;
			break;
		default:
			break;
		}

		if (!eventPropagation) {
			e.setTarget(this);
		}

		return eventPropagation;

	});
}

void ska::Button::display() const {
	if (!m_drawStyle) {
		return;
	}

	const auto& pos = getAbsolutePosition();
	m_textureSelector->render(pos.x, pos.y);
}



ska::Button::~Button() {

}
