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

	//m_guiObservable.HoverObservable::addObserver(*this);
	//m_guiObservable.ClickObservable::addObserver(*this);
	initHandlers();
	addClickHandler(callback);
}

ska::Button::Button() : 
m_state(ska::ButtonState::NONE),
m_drawStyle(false) {
	initHandlers();
}

ska::Button::Button(Widget& parent) : 
Widget(parent),
m_state(ska::ButtonState::NONE),
m_drawStyle(false) {
	initHandlers();
}

void ska::Button::initHandlers() {
	addHoverHandler([this](ska::HoverEvent& e) {
		bool eventPropagation = true;
		switch (e.getState()) {
		case ska::MouseEventType::MOUSE_ENTER:
			if (m_state != ButtonState::HOVER && m_state != ButtonState::PRESSED) {
				m_state = ButtonState::HOVER;
				m_textureSelector = &m_placeHolderHover;
				eventPropagation = false;
			}
			break;

		case ska::MouseEventType::MOUSE_OUT:
			if (m_state == ButtonState::HOVER) {
				m_state = ButtonState::NONE;
				m_textureSelector = &m_placeHolder;
				eventPropagation = false;
			}
			break;
		case ska::MouseEventType::MOUSE_OVER:
			//m_state = ButtonState::HOVER;
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

	addClickHandler([this](ska::ClickEvent& e) {
		bool eventPropagation = true;
		switch (e.getState()) {
		case ska::MouseEventType::MOUSE_CLICK:
			m_state = ButtonState::PRESSED;
			//m_textureSelector = &m_placeHolderPressed;
			eventPropagation = false;
			break;
		case ska::MouseEventType::MOUSE_RELEASE:
			m_state = ButtonState::HOVER;
			//m_callback(e);
			//m_textureSelector = &m_placeHolder;
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

/*void ska::Button::refresh() {
	const ska::InputRange& mousePos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];
	const ska::InputActionContainer& in = m_playerICM.getActions();
	
	ska::Rectangle buf;
	buf.x = m_relativePos.x + m_parent.getRect().x;
	buf.y = m_relativePos.y + m_parent.getRect().y;

	bool click = in[ska::InputAction::LClic];
	if (ska::RectangleUtils::isPositionInBox(mousePos, buf)) {
		if (click) {
			m_state = ButtonState::PRESSED;
			m_callback();
		} else {
			m_state = ButtonState::HOVER;
		}
	} else {
		m_state = ButtonState::NONE;
	}
}*/


ska::Button::~Button() {
	//m_guiObservable.HoverObservable::removeObserver(*this);
	//m_guiObservable.ClickObservable::removeObserver(*this);
}
