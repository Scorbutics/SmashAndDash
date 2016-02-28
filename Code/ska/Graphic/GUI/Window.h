#ifndef DEF_WINDOW
#define DEF_WINDOW


#include <windows.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include "../Rectangle.h"

namespace ska {
	class Window
	{
	public:
		Window();
		void pause();
		SDL_Renderer* getRenderer();
		~Window();
		unsigned int getWidth();
		unsigned int getHeight();
		SDL_Window* getHandle();
		void flip();
		void setNextFrameFlip(bool b);

	protected:
		std::string m_wName, m_bFilename;
		int m_loFenetre, m_laFenetre;
		bool m_flip;

	protected:
		SDL_Renderer *m_renderer;
		SDL_Texture* m_texture;
		SDL_Surface *m_bmp;
		SDL_Window *m_screen;
		ska::Rectangle m_positionFond;

	};
}
#endif
