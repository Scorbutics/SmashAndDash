#include "SDL.h"
#include "RawInputListener.h"
#include "../Exceptions/TerminateProcessException.h"
#include "../Utils/StringUtils.h"

const ska::KeyInput& ska::RawInputListener::getKeyInput() const {
    return m_keyIn;
}

const ska::MouseInput& ska::RawInputListener::getMouseInput() const {
	return m_mouseIn;
}

const std::wstring& ska::RawInputListener::getTextInput() const {
	return m_textInput;
}

const ska::WindowInput& ska::RawInputListener::getWindowInput() const {
	return m_windowInput;
}

void ska::RawInputListener::update() {
    SDL_Event event;
	m_mouseIn.setMouseLastPos(m_mouseIn.getMousePos());

	m_textInput = L"";
	m_keyIn.resetTriggers();
	m_mouseIn.resetTriggers();

	while(SDL_PollEvent(&event)) {
		switch (event.type) {
            case SDL_KEYDOWN:
                m_keyIn.setKeyState(event.key.keysym.scancode, true);
                break;
            case SDL_KEYUP:
				m_keyIn.setKeyState(event.key.keysym.scancode, false);
                break;
            case SDL_MOUSEMOTION:
				m_mouseIn.setMousePos(event);
                break;
            case SDL_MOUSEBUTTONDOWN:
				m_mouseIn.setMouseState(event.button.button, 1);
                break;
            case SDL_MOUSEBUTTONUP:
				m_mouseIn.setMouseState(event.button.button, 0);
                break;
			case SDL_TEXTEDITING:
				break;
			case SDL_TEXTINPUT:
				m_textInput += StringUtils::toUTF8(event.edit.text);
				break;
            case SDL_QUIT:
				throw TerminateProcessException("Program quitted");
		default:
                break;
		}

		if(event.type == SDL_WINDOWEVENT) {
			switch(event.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					m_windowInput.setSize(event.window.data1, event.window.data2);
					break;

				default:
					break;
			}
		}
	}

}
