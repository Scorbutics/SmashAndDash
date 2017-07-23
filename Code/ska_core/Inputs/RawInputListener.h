#pragma once

#include "KeyInput.h"
#include "MouseInput.h"
#include "WindowInput.h"

namespace ska {
	class RawInputListener {
	public:
		RawInputListener() = default;
		const KeyInput& getKeyInput() const;
		const MouseInput& getMouseInput() const;
		const std::wstring& getTextInput() const;
		const WindowInput& getWindowInput() const;
		void update();
		virtual ~RawInputListener() = default;

	private:
		WindowInput m_windowInput;
		KeyInput m_keyIn;
		MouseInput m_mouseIn;
		std::wstring m_textInput;
	};
}

