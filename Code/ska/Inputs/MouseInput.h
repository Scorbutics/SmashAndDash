#ifndef DEF_MOUSEINPUT
#define DEF_MOUSEINPUT

#include "../Graphic/Point.h"

namespace ska {
	class MouseInput
	{
	public:
		MouseInput();

		void setMouseLastPos(Point<int> mouselastpos);
		void setMousePos(SDL_Event event);

		void setMouseState(int touche, int x);
		bool mouseClick(int touche);

		Point<int> getMouseClickPos();
		Point<int> getMouseLastPos();
		Point<int> getMousePos();
		Point<int> getMouseTranslation();
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
		Point<int> m_clickPos, m_mouseLastPos;
	};
}
#endif