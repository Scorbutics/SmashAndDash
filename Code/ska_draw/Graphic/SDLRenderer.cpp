#include <iostream>
#include "SDLRenderer.h"
#include "SDLSurface.h"
#include "SDLTexture.h"
#include "Exceptions/IllegalArgumentException.h"

ska::SDLRenderer::SDLRenderer(SDL_Window * window, int index, Uint32 flags) :
    m_renderer(nullptr) {
    load(window, index, flags);
}

ska::SDLRenderer::SDLRenderer() :
    m_renderer(nullptr) {
}

ska::SDLRenderer::SDLRenderer(SDLRenderer&& r) :
    m_renderer(nullptr) {
    operator=(std::move(r));
}

ska::SDLRenderer& ska::SDLRenderer::operator=(SDLRenderer&& r) {
    m_renderer = std::move(r.m_renderer);
    return *this;
}

void ska::SDLRenderer::load(SDL_Window* window, int index, Uint32 flags) {
    if(m_renderer != nullptr) {
        free();
    }

    m_renderer = SDL_CreateRenderer(window, index, flags);

    if(m_renderer == nullptr) {
        std::cerr << "Erreur lors de la création de la fenêtre SDL (renderer) :" << SDL_GetError() << std::endl;
        throw IllegalArgumentException("Bad instanciation : renderer cannot be null");
    }
}

void ska::SDLRenderer::renderPresent() const {
    SDL_RenderPresent(m_renderer);
}

SDL_Texture* ska::SDLRenderer::createTextureFromSurface(const SDLSurface& s) const {
    return SDL_CreateTextureFromSurface(m_renderer, s.m_surface);
}

int ska::SDLRenderer::renderCopy(const SDLTexture& t, const Rectangle* clip, const Rectangle& dest) const {
    return SDL_RenderCopy(m_renderer, t.m_texture, clip, &dest);
}

void ska::SDLRenderer::free() {
    SDL_DestroyRenderer(m_renderer);
	m_renderer = nullptr;
}

ska::SDLRenderer::~SDLRenderer() {
    free();
}
