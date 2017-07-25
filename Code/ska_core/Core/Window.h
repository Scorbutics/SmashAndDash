#pragma once

#include <SDL.h>
#include "../Draw/SDLRenderer.h"

namespace ska {
	class Window {
	public:
		Window(const std::string& title, unsigned int w, unsigned int h);
		Window& operator=(const Window&) = delete;
		virtual ~Window();

        unsigned int getWidth() const;
		unsigned int getHeight() const;
		void resize(unsigned int w, unsigned int h);

		void showMessageBox(Uint32 flags, const std::string& title, const std::string& message) const ;
		void display() const;
		SDLRenderer& getRenderer();
		void setRenderColor(const Color& color) const;

	private:
		unsigned int m_height;
		unsigned int m_width;

		const std::string m_wName;

		SDL_Window * m_screen;
		SDLRenderer m_renderer;

		bool m_containsDefaultRenderer;

	};
}
