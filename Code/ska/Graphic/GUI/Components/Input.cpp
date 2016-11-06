#include <iostream>
#include <fstream>
#include <string>

#include <SDL2/SDL.h>
#include "Input.h"
#include "../../../Utils/StringUtils.h"

ska::Input::Input(Widget& parent, const std::string& text, int fontSize, ska::Point<int> relativePos) :
	Widget(parent, relativePos), 
	m_text(*this, text, fontSize, ska::Point<int>()),
	m_keyFocus(false),
	m_field(parent, relativePos, Button::MENU_DEFAULT_THEME_PATH + "textfield", nullptr, [&](ska::Widget* tthis, ska::ClickEvent& e) {
		if(!m_keyFocus && e.getState() == ska::MouseEventType::MOUSE_CLICK) {
			m_keyFocus = true;
			SDL_StartTextInput();
		}
	}) {

	addKeyHandler([&](ska::Widget* tthis, ska::KeyEvent& e) {
		m_rawText += e.getText();
		m_text.modifyText(ska::StringUtils::toANSI(m_rawText));
	});

}

bool ska::Input::isAffectedBy(const ska::KeyEvent& e) const {
	return true;
}

void ska::Input::display() const {
	m_field.display();
	m_text.display();
}

