#pragma once

#include "KeyInput.h"
#include "MouseInput.h"
#include "../Graphic/Rectangle.h"

namespace ska {
	class RawInputListener {
	public:
		RawInputListener() = default;
		KeyInput& getKeyInput();
		MouseInput& getMouseInput();
		const std::wstring& getTextInput() const;
		void update();
		virtual ~RawInputListener() = default;

	private:
		KeyInput m_keyIn;
		MouseInput m_mouseIn;
		std::wstring m_textInput;
	};
}

