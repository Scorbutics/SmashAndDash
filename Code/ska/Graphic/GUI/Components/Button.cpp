#include "Button.h"


ska::Button::Button(Widget& parent, ska::Point<int> relativePos, const std::string& placeHolderStyleName, ClickEventHandler const& callback) :
Widget(parent),
m_placeHolder(placeHolderStyleName + ".png"),
m_placeHolderHover(placeHolderStyleName + "_hover.png"),
m_placeHolderPressed(placeHolderStyleName + "_pressed.png"),
m_state(ButtonState::NONE),
m_callback(callback) {
	move(getAbsolutePosition() + relativePos);

	m_textureSelector = &m_placeHolderHover;
	setWidth(m_placeHolder.getWidth());
	setHeight(m_placeHolder.getHeight());
}

void ska::Button::display() const {
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
	m_state = ButtonState::PRESSED;
	m_textureSelector = &m_placeHolderPressed;
}

void ska::Button::release(const ska::ClickEvent& e) {
	m_state = ButtonState::NONE;
	m_callback(e);
	m_textureSelector = &m_placeHolder;
}

void ska::Button::mouseHover() {
	m_state = ButtonState::HOVER;
	m_textureSelector = &m_placeHolderHover;
}

void ska::Button::mouseOut() {
	m_state = ButtonState::NONE;
	m_textureSelector = &m_placeHolder;
}

ska::Button::~Button() {
}
