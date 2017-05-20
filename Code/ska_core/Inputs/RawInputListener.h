#pragma once

#include "KeyInput.h"
#include "MouseInput.h"

namespace ska {
	class RawInputListener {
	public:
		RawInputListener() = default;
		const KeyInput& getKeyInput() const;
		const MouseInput& getMouseInput() const;
		const std::wstring& getTextInput() const;
		void update();
		virtual ~RawInputListener() = default;

	private:
		KeyInput m_keyIn;
		MouseInput m_mouseIn;
		std::wstring m_textInput;
	};
}

