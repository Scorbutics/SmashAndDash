#include <iostream>
#include "SDLRenderer.h"
#include "SDLSurface.h"
#include "SDLTexture.h"
#include "../Exceptions/IllegalArgumentException.h"
#include "../Logging/Logger.h"

ska::SDLRenderer* ska::SDLRenderer::m_currentDefaultRenderer = nullptr;

ska::SDLRenderer::SDLRenderer(SDL_Window * window, int index, Uint32 flags) :
    m_renderer(nullptr) {
    load(window, index, flags);
}

ska::SDLRenderer::SDLRenderer() :
    m_renderer(nullptr) {
}

void ska::SDLRenderer::setRenderColor(const ska::Color & c) const{
	SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
}

void ska::SDLRenderer::setDefaultRenderer(SDLRenderer* r) {
	m_currentDefaultRenderer = r;
}

void ska::SDLRenderer::load(SDL_Window* window, int index, Uint32 flags) {
    if(m_renderer != nullptr) {
        free();
    }

    m_renderer = SDL_CreateRenderer(window, index, flags);
    SDL_SetRenderDrawColor( m_renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    if(m_renderer == nullptr) {
		SKA_LOG_ERROR("Erreur lors de la création de la fenêtre SDL (renderer) :", SDL_GetError());
        throw IllegalArgumentException("Bad instanciation : renderer cannot be null");
    }
}

void ska::SDLRenderer::renderClear() const {
    SDL_RenderClear(m_renderer);
}

void ska::SDLRenderer::renderPresent() const {
    SDL_RenderPresent(m_renderer);
}

SDL_Texture* ska::SDLRenderer::createTextureFromSurface(const SDLSurface& s) const {
    return SDL_CreateTextureFromSurface(m_renderer, s.m_surface);
}

ska::SDLRenderer* ska::SDLRenderer::getDefaultRenderer() {
    return m_currentDefaultRenderer;
}

int ska::SDLRenderer::renderCopy(const SDLTexture& t, const Rectangle* clip, const Rectangle& dest) const {
    return SDL_RenderCopy(m_renderer, t.m_texture, clip, &dest);
}

void ska::SDLRenderer::drawColorPoint(const Color& c, const Point<int>& pos) const {
	SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
	SDL_RenderDrawPoint(m_renderer, pos.x, pos.y);
}

void ska::SDLRenderer::drawColorRect(const Color& c, const Rectangle& r) const {
	SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
	SDL_RenderFillRect(m_renderer, &r);
}

void ska::SDLRenderer::free() {
    SDL_DestroyRenderer(m_renderer);
	m_renderer = nullptr;
}

ska::SDLRenderer::~SDLRenderer() {
    free();
}
