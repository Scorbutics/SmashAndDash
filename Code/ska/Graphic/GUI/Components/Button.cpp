#include <iostream>
#include "Button.h"
#include "ClickEvent.h"
#include "../Components/MouseObservable.h"

const std::string& ska::Button::MENU_DEFAULT_THEME_PATH = "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"default_theme"FILE_SEPARATOR;

ska::Button::Button(Widget& parent, ska::Point<int> relativePos, const std::string& placeHolderStyleName, const ska::Rectangle* clip, ClickEventHandler const& callback) :
Widget(parent),
m_state(ska::ButtonState::NONE),
m_placeHolder(placeHolderStyleName + ".png"),
m_placeHolderHover(placeHolderStyleName + "_hover.png"),
m_placeHolderPressed(placeHolderStyleName + "_pressed.png"),
m_drawStyle(!placeHolderStyleName.empty()) {
	move(getRelativePosition() + relativePos);
	
	m_textureSelector = &m_placeHolder;
	m_lastTextureSelector = nullptr;
	setWidth(m_placeHolder.getWidth());
	setHeight(m_placeHolder.getHeight());

	initHandlers();
	addClickHandler(callback);

	if (clip != nullptr) {
		m_clip = *clip;
	} else {
		memset(&m_clip, 0, sizeof m_clip);
		m_clip.w = std::numeric_limits<int>().max();
		m_clip.h = std::numeric_limits<int>().max();
	}

}

ska::Button::Button() : 
m_state(ska::ButtonState::NONE),
m_drawStyle(false) {
	m_textureSelector = &m_placeHolder;
	m_lastTextureSelector = nullptr;
	initHandlers();
	memset(&m_clip, 0, sizeof m_clip);
	m_clip.w = std::numeric_limits<int>().max();
	m_clip.h = std::numeric_limits<int>().max();
}

ska::Button::Button(Widget& parent) : 
Widget(parent),
m_state(ska::ButtonState::NONE),
m_drawStyle(false) {
	m_textureSelector = &m_placeHolder;
	m_lastTextureSelector = nullptr;
	initHandlers();
	memset(&m_clip, 0, sizeof m_clip);
	m_clip.w = std::numeric_limits<int>().max();
	m_clip.h = std::numeric_limits<int>().max();
}

void ska::Button::switchTextureAndMemorize(const ska::Texture& t) {
	m_lastTextureSelector = m_textureSelector;
	m_textureSelector = &t;
}

void ska::Button::initHandlers() {
	addHoverHandler([this](ska::Widget* tthis, ska::HoverEvent& e) {
		bool handled = true;
		bool target = false;
		switch (e.getState()) {
		case ska::MouseEventType::MOUSE_ENTER:
			if (m_state != ButtonState::HOVER && m_state != ButtonState::ENTER && m_state != ButtonState::PRESSED) {
				m_state = ButtonState::ENTER;
				switchTextureAndMemorize(m_placeHolderHover);
				target = true;
			} else {
				handled = false;
			}
			break;

		case ska::MouseEventType::MOUSE_OUT:
			if (m_state == ButtonState::HOVER || m_state == ButtonState::ENTER || m_state == ButtonState::PRESSED) {
				m_state = ButtonState::NONE;
				switchTextureAndMemorize(m_placeHolder);
				target = true;
			} else {
				handled = false;
			}
			break;

		case ska::MouseEventType::MOUSE_OVER:
			m_state = ButtonState::HOVER;
			target = true;
			break;

		default:
			break;

		}

		if (!handled) {
			/* Refuses the current event (not handled) */
			e.stopPropagation(ska::StopType::STOP_CALLBACK);
		}

		if (target && e.getTarget() == nullptr) {
			e.setTarget(this);
		}

		//return stopEventPropagation;
	});

	addClickHandler([this](ska::Widget* tthis, ska::ClickEvent& e) {
		bool handled = false;
		switch (e.getState()) {
		case ska::MouseEventType::MOUSE_CLICK:
			m_state = ButtonState::PRESSED;
			switchTextureAndMemorize(m_placeHolderPressed);
			handled = true;
			break;
		case ska::MouseEventType::MOUSE_RELEASE:
			m_state = ButtonState::HOVER;
			m_textureSelector = m_lastTextureSelector;
			handled = true;
			break;
		default:
			break;
		}

		if (handled) {
			e.setTarget(this);
		} else {
			e.stopPropagation(ska::StopType::STOP_CALLBACK);
		}
	});
}

void ska::Button::display() const {
	if (!m_drawStyle) {
		return;
	}

	const auto& pos = getAbsolutePosition();
	if (m_clip.w == std::numeric_limits<int>().max()) {
		m_textureSelector->render(pos.x, pos.y);
	} else {
		m_textureSelector->render(pos.x, pos.y, &m_clip);
	}
}



ska::Button::~Button() {

}

