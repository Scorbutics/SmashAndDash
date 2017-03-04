#include <iostream>
#include <string>

#include "Texture.h"
#include "../Exceptions/IllegalStateException.h"
#include "./GUI/Window.h"

ska::Window* ska::Texture::m_window = NULL;

ska::Texture::Texture(std::string id, int r, int g, int b, int a) : ResourceTemplate() {
	load(id, r, g, b, a);
}


ska::Texture::Texture() : ResourceTemplate() {
}

void ska::Texture::freeAll() {
	m_container.clear();
}


void ska::Texture::load(std::string id, int r, int g, int b, int a) {
	checkWindow();

	SDL_Color finalColor;
	finalColor.a = a;
	finalColor.b = b;
	finalColor.g = g;
	finalColor.r = r;

	ResourceTemplate::load(TextureData(*m_window, id, finalColor));
}

void ska::Texture::setColor(Uint8 r, Uint8 g, Uint8 b)
{
	if (m_value != NULL) {
		SDL_SetTextureColorMod(m_value->m_texture, r, g, b);
	}
}

void ska::Texture::setBlendMode(SDL_BlendMode blending) {
	if (m_value != NULL) {
		SDL_SetTextureBlendMode(m_value->m_texture, blending);
	}
}

void ska::Texture::operator=(const Texture& text) {
	m_key = text.m_key;
	m_value = text.m_value;
}

void ska::Texture::setAlpha(Uint8 alpha) {
	if (m_value != NULL) {
		SDL_SetTextureAlphaMod(m_value->m_texture, alpha);
		m_value->m_alpha = alpha;
	}
}

void ska::Texture::setDefaultWindow(Window* w) {
	m_window = w;
}

int ska::Texture::render(int x, int y, const Rectangle* clip) const {
	if (m_value == NULL) {
		return -1;
	}

	checkWindow();

	Rectangle destBuf = { x, y, m_value->m_w, m_value->m_h };

	if( clip != NULL ) {
		destBuf.w = clip->w;
		destBuf.h = clip->h;
	}

	return SDL_RenderCopy(m_window->getRenderer(), m_value->m_texture, clip, &destBuf);
}

void ska::Texture::checkWindow() {
	if (m_window == NULL) {
		throw IllegalStateException("The Texture's default window is not set");
	}
}

void ska::Texture::loadFromText(unsigned int fontSize, std::string text, Color c) {
	checkWindow();

	m_key = TextureData(*m_window, text, c);
	m_value = NULL;
	if (m_container.find(m_key) == m_container.end() || m_container[m_key].lock() == NULL) {
		m_value = std::shared_ptr<SDLTexture>(new SDLTexture());
		m_container[m_key] = m_value;
		m_value->loadFromText(*m_window, fontSize, text, c);
	} else {
		m_value = m_container[m_key].lock();
	}

}

const unsigned int ska::Texture::getWidth() const {
	return m_value == NULL ? 0 : m_value->m_w;
}

const unsigned int ska::Texture::getHeight() const {
	return m_value == NULL ? 0 : m_value->m_h;
}


ska::Texture::~Texture() {
	free();
}


