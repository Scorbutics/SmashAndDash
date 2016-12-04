#pragma once

#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include "../Rectangle.h"

namespace ska {
	class Window {
	public:
		Window(const std::string& title, const unsigned int w, const unsigned int h);
		SDL_Renderer* getRenderer() const;
		~Window();
		unsigned int getWidth() const;
		unsigned int getHeight() const;
		SDL_Window* getHandle();
		void flip();
		void resize(unsigned int w, unsigned int h);

	private:
		const std::string& m_wName;
		unsigned int m_height;
		unsigned int m_width;
	
	protected:
		SDL_Renderer *m_renderer;
		SDL_Window *m_screen;

	};
}
