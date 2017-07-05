#include <iostream>
#include <sstream>
#include <SDL_image.h>
#include "Font.h"
#include "SDLSurface.h"

SDL_Surface* LoadImage32(const std::string& fichier_image);

SDL_Surface* LoadImage32(const std::string& fichier_image) {
	SDL_Surface* result = nullptr;
	SDL_Surface* imageRam;
	imageRam = IMG_Load(fichier_image.c_str());
    if(imageRam != nullptr) {
        result = SDL_CreateRGBSurface(0, imageRam->w, imageRam->h, 32, 0, 0, 0, 0);
        if (result == nullptr) {
			goto loadImage32Free;
        }

        /* Copie l'image image_ram de moins de 32 bits vers image_result qui fait 32 bits */
        SDL_UpperBlit(imageRam, nullptr, result, nullptr);

		/* Clean up phase */
		loadImage32Free:
		SDL_FreeSurface(imageRam);
		imageRam = nullptr;
	}

	return result;
}

ska::SDLSurface::SDLSurface() {
	m_surface = nullptr;
}

const SDL_PixelFormat * ska::SDLSurface::getFormat() const {
	return m_surface == nullptr ? nullptr : m_surface->format;
}

SDL_Surface* ska::SDLSurface::getInstance() const {
	return m_surface;
}

void ska::SDLSurface::loadFromText(const Font& font, const std::string& text, Color c) {
	free();
	if(font.getInstance() == nullptr) {
		throw ska::IllegalArgumentException("Invalid font provided to a surface");
	}
	m_surface = TTF_RenderText_Blended(font.getInstance(), text.c_str(), c.toNative());
}

void ska::SDLSurface::load(const std::string& file) {
	free();
	m_surface = IMG_Load(file.c_str());
}

void ska::SDLSurface::load32(const std::string& file) {
	free();
	m_surface = LoadImage32(file);
}

ska::Color ska::SDLSurface::getPixel32Color(int x, int y) const {
	if (m_surface == nullptr) {
		return Color( 0, 0, 0, 0 );
	}

	Color c;
	c.a = 0;
	Uint32 pix = getPixel32(x, y);
	SDL_GetRGB(pix, getFormat(), &c.r, &c.g, &c.b);
	return c;
}

Uint32 ska::SDLSurface::getPixel32(int x, int y) const {
	if (m_surface == nullptr || x < 0 || x > m_surface->w - 1 || y < 0 || y > m_surface->h - 1) {
		return 0;
	}
	return ((Uint32*)(m_surface->pixels))[y*(m_surface->pitch / 4) + x];
}


void ska::SDLSurface::free() {
	SDL_FreeSurface(m_surface);
	m_surface = nullptr;
}

ska::SDLSurface::~SDLSurface() {
	free();
}
