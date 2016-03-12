#include <iostream>
#include <string>

#include "Texture.h"
#include "../Exceptions/IllegalStateException.h"

using namespace std;

ska::Window* ska::Texture::m_window = NULL;

ska::Texture::Texture(string id, int r, int g, int b, int a) : ResourceTemplate()
{
	load(id, r, g, b, a);
}


ska::Texture::Texture() : ResourceTemplate()
{
}

void ska::Texture::freeAll()
{
	m_container.clear();
}


void ska::Texture::load(string id, int r, int g, int b, int a)
{
	checkWindow();

	SDL_Color finalColor;
	finalColor.a = a;
	finalColor.b = b;
	finalColor.g = g;
	finalColor.r = r;

	ResourceTemplate::load(ska::TextureData(*m_window, id, finalColor));
}

void ska::Texture::setColor(Uint8 r, Uint8 g, Uint8 b)
{
	if (m_value != NULL) {
		SDL_SetTextureColorMod(m_value->m_texture, r, g, b);
	}
}

void ska::Texture::setBlendMode(SDL_BlendMode blending)
{
	if (m_value != NULL) {
		SDL_SetTextureBlendMode(m_value->m_texture, blending);
	}
}

void ska::Texture::operator=(const ska::Texture& text) {
	m_key = text.m_key;
	m_keyStr = text.m_keyStr;
	m_value = text.m_value;
}

void ska::Texture::setAlpha(Uint8 alpha)
{
	if (m_value != NULL) {
		SDL_SetTextureAlphaMod(m_value->m_texture, alpha);
		m_value->m_alpha = alpha;
	}
}

void ska::Texture::setDefaultWindow(ska::Window* w) {
	m_window = w;
}

int ska::Texture::render(int x, int y, ska::Rectangle* clip)
{
	if (m_value == NULL) {
		return -1;
	}

	checkWindow();

	ska::Rectangle destBuf = { x, y, m_value->m_w, m_value->m_h };

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

void ska::Texture::loadFromText(unsigned int fontSize, string text, SDL_Color c)
{
	checkWindow();

	m_key = ska::TextureData(*m_window, text, c);
	m_keyStr = m_key.toString();
	m_value = NULL;
	if (m_container.find(m_keyStr) == m_container.end() || m_container[m_keyStr].lock() == NULL) {
		m_value = std::shared_ptr<ska::SDLTexture>(new ska::SDLTexture());
		m_container[m_keyStr] = m_value;
		m_value->loadFromText(*m_window, fontSize, text, c);
	} else {
		m_value = m_container[m_keyStr].lock();
	}

}

int ska::Texture::getWidth() const
{
	return m_value == NULL ? 0 : m_value->m_w;
}

int ska::Texture::getHeight() const
{
	return m_value == NULL ? 0 : m_value->m_h;
}


ska::Texture::~Texture()
{
	free();
}


