#include <iostream>
#include <string>
#include <fstream>
#include "../../Utils/SkaConstants.h"
#include "Window.h"
#include "../Texture.h"

#define TAILLEBLOCFENETRE 32
#define TAILLEECRANMINX TAILLEBLOCFENETRE*15
#define TAILLEECRANMINY TAILLEBLOCFENETRE*13


ska::Window::Window() : 
m_width(TAILLEECRANMINX * 2),
m_height(TAILLEECRANMINY * 3 / 2) {
	Texture::setDefaultWindow(this);
	m_wName = "Fenetre";

	m_screen = SDL_CreateWindow("Pokémon Smash&Dash",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_width, m_height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

	if (m_screen == NULL) {
		std::cerr << "Erreur lors de la création de la fenêtre SDL :" << SDL_GetError() << std::endl;
	}
	
	m_renderer = SDL_CreateRenderer(m_screen, -1, SDL_RENDERER_ACCELERATED);
	
	if (m_renderer == NULL) {
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

void ska::Window::flip() {	
	SDL_RenderPresent(m_renderer);
}


SDL_Window* ska::Window::getHandle() {
	return m_screen;
}


ska::Window::~Window() {
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_screen);
}
