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

	/* Propagation du handler vers le composant m_field */
	addClickHandler([&](ska::Widget* tthis, ska::ClickEvent& e) {
		m_field.click(e);
	});

	addKeyHandler([&](ska::Widget* tthis, ska::KeyEvent& e) {
		if(e.getState() == ska::KeyEventType::KEY_DOWN) {
			if (m_rawText.size() > 1 && e.getScanCode() == SDL_SCANCODE_BACKSPACE) {
				m_rawText.pop_back();
			} else {
				m_rawText = L" ";
			}
		} else {
			m_rawText += e.getText();
		}
		m_text.modifyText(ska::StringUtils::toANSI(m_rawText));
	});

	setWidth(m_field.getBox().w);
	setHeight(m_field.getBox().h);
}

bool ska::Input::isAffectedBy(const ska::KeyEvent& e) const {
	return true;
}

void ska::Input::display() const {
	m_field.display();
	m_text.display();
}

