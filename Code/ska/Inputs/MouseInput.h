#ifndef DEF_MOUSEINPUT
#define DEF_MOUSEINPUT

#include "../Graphic/Point.h"
#include "InputEvents.h"

namespace ska {
	class MouseInput
	{
	public:
		MouseInput();

		void setMouseLastPos(ska::Point<int> mouselastpos);
		void setMousePos(SDL_Event event);

		void setMouseState(int touche, int x);
		bool mouseClick(int touche);

		ska::Point<int> getMouseClickPos();
		ska::Point<int> getMouseLastPos();
		ska::Point<int> getMousePos();
		ska::Point<int> getMouseTranslation();
		int trigger(int touche);
		int toggle(int touche);
		
		void resetTriggers();
		void resetAll();

		~MouseInput();

	private:
		int m_mousex, m_mousey;
		int m_mousexrel, m_mouseyrel;
		
		int m_mouseState[8];
		int m_mouseToggle[8];
		ska::Point<int> m_clickPos, m_mouseLastPos;
	};
}
#endif