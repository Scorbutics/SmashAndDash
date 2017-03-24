#include <iostream>
#include <string>
#include <fstream>

#include "Window.h"
#include "../../Texture.h"


#define TAILLEBLOCFENETRE 32
#define TAILLEECRANMINX TAILLEBLOCFENETRE*15
#define TAILLEECRANMINY TAILLEBLOCFENETRE*13


ska::Window::Window(const std::string& title, const unsigned int w, const unsigned int h) :
m_wName(title),
m_height(h < TAILLEECRANMINY ? TAILLEECRANMINY : h),
m_width(w < TAILLEECRANMINX ? TAILLEECRANMINX : w) {
	Texture::setDefaultWindow(this);

	m_screen = SDL_CreateWindow(title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_width, m_height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

	if (m_screen == nullptr) {
		std::cerr << "Erreur lors de la création de la fenêtre SDL :" << SDL_GetError() << std::endl;
	}

	m_renderer = SDL_CreateRenderer(m_screen, -1, SDL_RENDERER_ACCELERATED);

	if (m_renderer == nullptr) {
		std::cerr << "Erreur lors de la création de la fenêtre SDL (renderer) :" << SDL_GetError() << std::endl;
	}

	SDL_SetRenderDrawColor( m_renderer, 0xFF, 0xFF, 0xFF, 0xFF );

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear")) {
		std::cerr << "Attention : Linear texture filtering inactivé !" << std::endl;
	}

}

unsigned int ska::Window::getWidth() const {
	return m_width;
}

unsigned int ska::Window::getHeight() const {
	return m_height;
}

SDL_Renderer* ska::Window::getRenderer() const {
	return m_renderer;
}

void ska::Window::flip() const{
	SDL_RenderPresent(m_renderer);
}


void ska::Window::resize(unsigned int w, unsigned int h) {
	m_width = w;
	m_height = h;
}

SDL_Window* ska::Window::getHandle() const{
	return m_screen;
}


ska::Window::~Window() {
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_screen);
}
