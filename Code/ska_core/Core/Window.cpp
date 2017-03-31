#include <iostream>
#include <string>
#include <fstream>
#include "../Exceptions/IllegalArgumentException.h"
#include "Window.h"
#include "../Draw/SDLRenderer.h"

#define TAILLEBLOCFENETRE 32
#define TAILLEECRANMINX TAILLEBLOCFENETRE*15
#define TAILLEECRANMINY TAILLEBLOCFENETRE*13

ska::Window::Window(const std::string& title, const unsigned int w, const unsigned int h) :
m_wName(title),
m_height(h < TAILLEECRANMINY ? TAILLEECRANMINY : h),
m_width(w < TAILLEECRANMINX ? TAILLEECRANMINX : w),
m_containsDefaultRenderer(false) {

	m_screen = SDL_CreateWindow(title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_width, m_height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

	if (m_screen == nullptr) {
		std::cerr << "Erreur lors de la création de la fenêtre SDL :" << SDL_GetError() << std::endl;
		throw IllegalArgumentException("Bad instanciation : screen cannot be null");
	}

	m_renderer.load(m_screen, -1, SDL_RENDERER_ACCELERATED);

	if(SDLRenderer::getDefaultRenderer() == nullptr) {
        m_containsDefaultRenderer = true;
        SDLRenderer::setDefaultRenderer(&m_renderer);
	}
}

unsigned int ska::Window::getWidth() const {
	return m_width;
}

unsigned int ska::Window::getHeight() const {
	return m_height;
}

void ska::Window::display() const{
	m_renderer.renderPresent();
	m_renderer.renderClear();
}

void ska::Window::showMessageBox(Uint32 flags, const std::string& title, const std::string& message) const {
    SDL_ShowSimpleMessageBox(flags,
            title.c_str(),
            message.c_str(),
            m_screen);
}

void ska::Window::resize(unsigned int w, unsigned int h) {
	m_width = w;
	m_height = h;
}

ska::Window::~Window() {
    if(m_containsDefaultRenderer) {
        SDLRenderer::setDefaultRenderer(nullptr);
    }
	SDL_DestroyWindow(m_screen);
}
