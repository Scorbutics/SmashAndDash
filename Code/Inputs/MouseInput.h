#ifndef DEF_MOUSEINPUT
#define DEF_MOUSEINPUT

#include "../Graphic/Rectangle.h"
#include "EventArg.h"

class MouseInput
{
	public:
		MouseInput();
		
		void setMouseLastPos(ska::Rectangle mouselastpos);
		void setMousePos(SDL_Event event);
		void setMouseLastState(int touche, int x);
		void setMouseState(int touche, int x);
		bool mouseClick(int touche);

		ska::Rectangle getMouseClickPos();
		ska::Rectangle getMouseLastPos();
		ska::Rectangle getMousePos();
		ska::Rectangle getMouseTranslation();
		int getMouseState(int touche);
		int getMouseLastState(int touche);
		
		void MouseInput::resetAll();

		~MouseInput();

	private:
		MouseEvent m_mouseEvent;
		int m_mousex, m_mousey;
		int m_mousexrel, m_mouseyrel;
		int m_mousebutton[8];
		int m_mouseLastState[8];
		ska::Rectangle m_clickPos, m_mouseLastPos;
};

#endif