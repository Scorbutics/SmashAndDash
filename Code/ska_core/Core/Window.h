#pragma once

#include <iostream>
#include <SDL.h>
#include "../Draw/SDLRenderer.h"
#include "BaseWindow.h"

namespace ska {
	class Window : public BaseWindow {
	public:
		Window(const std::string& title, unsigned int w, unsigned int h);
		Window& operator=(const Window&) = delete;
		virtual ~Window();

		void showMessageBox(Uint32 flags, const std::string& title, const std::string& message) const ;
		void display() const;

	private:
		const std::string m_wName;

		SDL_Window * m_screen;
		SDLRenderer m_renderer;

		bool m_containsDefaultRenderer;

	};
}
