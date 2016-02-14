#ifndef DEF_MOUSEINPUT
#define DEF_MOUSEINPUT

#include <SDL2/SDL.h>
#include "EventArg.h"

class MouseInput
{
	public:
		MouseInput();
		
		void setMouseLastPos(SDL_Rect mouselastpos);
		void setMousePos(SDL_Event event);
		void setMouseLastState(int touche, int x);
		void setMouseState(int touche, int x);
		bool mouseClick(int touche);

		SDL_Rect getMouseClickPos();
		SDL_Rect getMouseLastPos();
		SDL_Rect getMousePos();
		SDL_Rect getMouseTranslation();
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
		SDL_Rect m_clickPos, m_mouseLastPos;
};

#endif