#include <iostream>
#include "Button.h"
#include "ClickEvent.h"
#include "../Components/MouseObservable.h"

const std::string& ska::Button::MENU_DEFAULT_THEME_PATH = "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"default_theme"FILE_SEPARATOR;

ska::Button::Button(Widget& parent, ska::Point<int> relativePos, const std::string& placeHolderStyleName, const ska::Rectangle* clip, ClickEventHandler const& callback) :
Hoverable<ClickEventListener>(parent),
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
	addHandler<ClickEventListener>(callback);

	if (clip != nullptr) {
		m_clip = *clip;
	} else {
		memset(&m_clip, 0, sizeof m_clip);
		m_clip.w = std::numeric_limits<int>().max();
		m_clip.h = std::numeric_limits<int>().max();
	}

}

ska::Button::Button() : 
m_drawStyle(false) {
	m_textureSelector = &m_placeHolder;
	m_lastTextureSelector = nullptr;
	memset(&m_clip, 0, sizeof m_clip);
	m_clip.w = std::numeric_limits<int>().max();
	m_clip.h = std::numeric_limits<int>().max();

	initHandlers();
}

ska::Button::Button(Widget& parent) : 
Hoverable<ClickEventListener>(parent),
m_drawStyle(false) {
	m_textureSelector = &m_placeHolder;
	m_lastTextureSelector = nullptr;
	memset(&m_clip, 0, sizeof m_clip);
	m_clip.w = std::numeric_limits<int>().max();
	m_clip.h = std::numeric_limits<int>().max();

	initHandlers();
}

void ska::Button::switchTextureAndMemorize() {
	ska::Texture* t = nullptr;
	switch (m_state) {
		case ska::ButtonState::HOVER:
		case ska::ButtonState::ENTER:
			t = &m_placeHolderHover;
			break;

		case ska::ButtonState::NONE:
		case ska::ButtonState::DISABLED:
			t = &m_placeHolder;
			break;
		
		case ska::ButtonState::PRESSED:
			t = &m_placeHolderPressed;
			break;

		default:
			t = &m_placeHolder;
			break;

	}
	m_lastTextureSelector = m_textureSelector;
	m_textureSelector = t;
}

void ska::Button::resetTexture() {
	m_textureSelector = m_lastTextureSelector;
}


void ska::Button::initHandlers() {
	addHeadHandler<ClickEventListener>([this](ska::Widget* tthis, ska::ClickEvent& e) {
		bool handled = false;
		switch (e.getState()) {
		case ska::MouseEventType::MOUSE_CLICK:
			m_state = ButtonState::PRESSED;
			switchTextureAndMemorize();
			handled = true;
			break;
		case ska::MouseEventType::MOUSE_RELEASE:
			m_state = ButtonState::HOVER;
			resetTexture();
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

