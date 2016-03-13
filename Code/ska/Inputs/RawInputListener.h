#ifndef DEF_KEYBOARD_LISTENER
#define DEF_KEYBOARD_LISTENER

#include "KeyInput.h"
#include "MouseInput.h"
#include "../Graphic/Rectangle.h"

int GetDirectionFromChar(char directionChar);
char GetCharFromDirection(int dir);

namespace ska {
	class RawInputListener
	{
	public:

		RawInputListener();
		KeyInput& getKeyInput();
		MouseInput& getMouseInput();
		void update();
		virtual ~RawInputListener();

	private:
		KeyInput m_keyIn;
		MouseInput m_mouseIn;

	};
}
#endif