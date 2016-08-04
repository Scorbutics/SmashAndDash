#pragma once

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
		SDL_Renderer* getRenderer() const;
		~Window();
		unsigned int getWidth() const;
		unsigned int getHeight() const;
		SDL_Window* getHandle();
		void flip();

	protected:
		std::string m_wName;
		unsigned int m_height;
		unsigned int m_width;

		SDL_Renderer *m_renderer;
		SDL_Texture* m_texture;
		SDL_Window *m_screen;

	};
}
