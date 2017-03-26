#pragma once

#include <iostream>
#include <SDL.h>
#include "Graphic/SDLRenderer.h"

namespace ska {
	class Window {
	public:
		Window(const std::string& title, const unsigned int w, const unsigned int h);
		Window& operator=(const Window&) = delete;

		virtual ~Window();
		unsigned int getWidth() const;
		unsigned int getHeight() const;

		void showMessageBox(Uint32 flags, const std::string& title, const std::string& message) const ;

		void flip() const;
		void resize(unsigned int w, unsigned int h);

	private:
		const std::string& m_wName;
		unsigned int m_height;
		unsigned int m_width;

	protected:
		SDL_Window * m_screen;
		SDLRenderer m_renderer;

	};
}
