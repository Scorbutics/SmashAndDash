

#include <SDL.h>
#include "Input.h"
#include "../../Events/KeyEvent.h"
#include "../../Events/ValueChangedEvent.h"
#include "Utils/StringUtils.h"


ska::Input::Input(Widget& parent, const std::string& text, int fontSize, Point<int> relativePos) :
	WidgetPanel<ValueChangedEventListener<std::wstring>, ClickEventListener, KeyEventListener, FocusEventListener>(parent, relativePos),
	m_keyFocus(false) {

	auto& button = addWidget<Button>(relativePos, Button::MENU_DEFAULT_THEME_PATH + "textfield", nullptr, [&](Widget* tthis, ClickEvent& e) {
		if (!m_keyFocus && e.getState() == MOUSE_CLICK) {
			m_keyFocus = true;
			SDL_StartTextInput();
		}
		e.setTarget(this);
		auto b = reinterpret_cast<Button*>(tthis);
		b->forceState(ButtonState::PRESSED);
	});

	auto& label = addWidget<Label>(text, fontSize, Point<int>(5, button.getBox().h / 2 - fontSize/2));
	Widget::setWidth(button.getBox().w);
	Widget::setHeight(button.getBox().h);

	m_clip.y = 0;
	m_clip.w = getBox().w - 10;
	adaptDisplayWithText(label);

	addHandler<FocusEventListener>([&](Widget*, FocusEvent& e) {
		auto f = e.getState() == MOUSE_FOCUS;
		focus(f);
		e.setTarget(this);
		auto b = reinterpret_cast<Button*>(getWidget(0));
		b->forceState(f ? ButtonState::PRESSED : ButtonState::NONE);

		if(!f) {
			ValueChangedEvent<std::wstring> vce(m_lastRawText, m_rawText);
			directNotify(vce);
		}
	});

	addHandler<KeyEventListener>([&](Widget*, KeyEvent& e) {
		if (!isFocused()) {
			return;
		}
		auto l = reinterpret_cast<Label*>(getWidget(1));
		m_lastRawText = m_rawText;
		if(e.getState() == KEY_DOWN) {
			if (m_rawText.size() > 1 && e.getScanCode() == SDL_SCANCODE_BACKSPACE) {
				m_rawText.pop_back();
			} else {
				m_rawText = L" ";
			}
		} else {
			if (m_rawText.size() == 1 && m_rawText[0] == ' ') {
				m_rawText.pop_back();
			}
			m_rawText += e.getText();
		}

		l->modifyText(StringUtils::toANSI(m_rawText));
		adaptDisplayWithText(*l);
	});


}

void ska::Input::adaptDisplayWithText(Label& l) {
	auto xLimit = getBox().w - 10;
	if (l.getBox().w > xLimit) {
		m_clip.x = l.getBox().w - xLimit;
		m_clip.h = l.getBox().h;
		l.setClip(&m_clip);
	} else {
		m_clip.x = 0;
		m_clip.h = l.getBox().h;
		l.setClip(nullptr);
	}

}

