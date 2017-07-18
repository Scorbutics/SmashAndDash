#include <iostream>
#include <SDL_ttf.h>
#include "SDLRenderer.h"
#include "../Utils/SkaConstants.h"
#include "SDLTexture.h"
#include "SDLSurface.h"
#include "Font.h"
#include "TextureData.h"
#include "../Exceptions/FileException.h"
#include "../Logging/Logger.h"

ska::SDLTexture::SDLTexture() : m_r(0), m_g(0), m_b(0), m_texture(nullptr), m_w(0), m_h(0), m_alpha(0){
}

ska::SDLTexture::SDLTexture(TextureData& data) : m_texture(nullptr) {
	load(data.getRenderer(), data.getData().first, data.getData().second.r, data.getData().second.g, data.getData().second.b, data.getData().second.a);
}

void ska::SDLTexture::load(const SDLRenderer& renderer, const std::string& fileName, int r, int g, int b, int a) {
	SDLSurface sprite;
	m_texture = nullptr;
	m_fileName = fileName;
	m_r = static_cast<Uint8>(r);
	m_g = static_cast<Uint8>(g);
	m_b = static_cast<Uint8>(b);

	m_w = m_h = 0;
	m_alpha = static_cast<Uint8>((a > 0 && a < 255) ? a : 255);

	sprite.load(m_fileName);

	if (sprite.getInstance() == nullptr) {
		SKA_LOG_ERROR("Erreur lors du chargement de l'image \"", m_fileName, "\" : ", SDL_GetError());
	} else {
		SDL_SetColorKey(sprite.getInstance(), SDL_TRUE, SDL_MapRGBA(sprite.getFormat(), m_r, m_g, m_b, m_alpha));
	}

	//255 : opaque, donc pas besoin d'appliquer de l'alpha
	//si négatif, cela veut dire qu'on ne veut pas d'alpha non plus
	if (m_alpha < 255) {
		SDL_SetSurfaceAlphaMod(sprite.getInstance(), m_alpha);
	}

	if (sprite.getInstance() == nullptr) {
		sprite.load(NOSUCHFILE);
	}

	if (sprite.getInstance() == nullptr) {
		throw FileException("Erreur du chargement de l'image " + m_fileName + ". Après tentative de récupération, impossible de charger l'image \"" NOSUCHFILE "\" : " + std::string(SDL_GetError()));
	}

	free();
	m_fileName = fileName;
	m_texture = renderer.createTextureFromSurface(sprite);
	m_w = sprite.getInstance()->w;
	m_h = sprite.getInstance()->h;
}

void ska::SDLTexture::loadFromText(const SDLRenderer& renderer, unsigned int fontSize, const std::string& text, Color c) {
	SDLSurface buffer;
	Font f(fontSize);

	buffer.loadFromText(f, text, c);

	if (buffer.getInstance() == nullptr) {
		throw FileException("Erreur lors de la création de l'image-texte : " + std::string(TTF_GetError()));
	}

	free();
	m_fileName = text;
	m_texture = renderer.createTextureFromSurface(buffer);
	m_h = buffer.getInstance()->h;
	m_w = buffer.getInstance()->w;
	m_alpha = c.a;

}

void ska::SDLTexture::free() {
	if (m_texture != nullptr) {
		SDL_DestroyTexture(m_texture);
		m_texture = nullptr;
	}
	m_fileName = NOSUCHFILE;
	m_h = m_w = 0;
	m_alpha = 255;
}

ska::SDLTexture::~SDLTexture() {
	free();
}
