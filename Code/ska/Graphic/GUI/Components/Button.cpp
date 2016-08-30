#include "Button.h"
#include "ClickEvent.h"
#include "../GUI.h"

ska::Button::Button(GUI& guiObservable, Widget& parent, ska::Point<int> relativePos, const std::string& placeHolderStyleName, ClickEventHandler const& callback) :
Widget(parent),
ClickObserver(std::bind(&ska::Button::click, this, std::placeholders::_1)),
HoverObserver(std::bind(&ska::Button::mouseHover, this, std::placeholders::_1)),
m_placeHolder(placeHolderStyleName + ".png"),
m_placeHolderHover(placeHolderStyleName + "_hover.png"),
m_placeHolderPressed(placeHolderStyleName + "_pressed.png"),
m_state(ska::ButtonState::NONE),
m_callback(callback),
m_guiObservable(guiObservable),
m_drawStyle(!placeHolderStyleName.empty()) {
	move(getRelativePosition() + relativePos);

	m_textureSelector = &m_placeHolderHover;
	setWidth(m_placeHolder.getWidth());
	setHeight(m_placeHolder.getHeight());

	m_guiObservable.HoverObservable::addObserver(*this);
	m_guiObservable.ClickObservable::addObserver(*this);
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

void ska::Button::click(const ska::ClickEvent& e) {
	if (e.getState() == ska::MouseEventType::MOUSE_CLICK && (m_state == ButtonState::HOVER)) {
		m_state = ButtonState::PRESSED;
		m_textureSelector = &m_placeHolderPressed;
	} else if (m_state == ButtonState::PRESSED){
		m_state = ButtonState::NONE;
		m_callback(e);
		m_textureSelector = &m_placeHolder;
	}
}
void ska::Button::mouseHover(const ska::HoverEvent& e) {
	if (e.getState() == ska::MouseEventType::MOUSE_HOVER && m_state == ButtonState::NONE) {
		m_state = ButtonState::HOVER;
		m_textureSelector = &m_placeHolderHover;
	} else if (m_state == ButtonState::HOVER) {
		m_state = ButtonState::NONE;
		m_textureSelector = &m_placeHolder;
	}
}

ska::Button::~Button() {
	m_guiObservable.HoverObservable::removeObserver(*this);
	m_guiObservable.ClickObservable::removeObserver(*this);
}