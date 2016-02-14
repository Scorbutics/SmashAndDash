#include <iostream>
#include <string>

#include "Texture.h"
#include "Gameplay\WGameCore.h"

using namespace std;


Texture::Texture(string id, int r, int g, int b, int a) : ResourceTemplate()
{
	load(id, r, g, b, a);
}


Texture::Texture() : ResourceTemplate()
{
}

void Texture::freeAll()
{
	m_container.clear();
}


void Texture::load(string id, int r, int g, int b, int a)
{
	SDL_Color finalColor;
	finalColor.a = a;
	finalColor.b = b;
	finalColor.g = g;
	finalColor.r = r;

	//unique_ptr<IStringAble> p = unique_ptr<IStringAble>(new TextureData(id, finalColor));
	ResourceTemplate::load(TextureData(id, finalColor));
}

void Texture::setColor( Uint8 r, Uint8 g, Uint8 b)
{
	if (m_value != NULL)
		SDL_SetTextureColorMod(m_value->m_texture, r, g, b );
}

void Texture::setBlendMode( SDL_BlendMode blending )
{
	if (m_value != NULL)
		SDL_SetTextureBlendMode(m_value->m_texture, blending);
}

void Texture::setAlpha( Uint8 alpha )
{
	if (m_value != NULL)
	{
		SDL_SetTextureAlphaMod(m_value->m_texture, alpha);
		m_value->m_alpha = alpha;
	}
}

int Texture::render( int x, int y, SDL_Rect* clip )
{
	if (m_value == NULL)
		return -1;

	WGameCore& wScreen = WGameCore::getInstance();
	SDL_Rect destBuf = { x, y, m_value->m_w, m_value->m_h };

	if( clip != NULL )
	{
		destBuf.w = clip->w;
		destBuf.h = clip->h;
	}


	return SDL_RenderCopy(wScreen.getRenderer(), m_value->m_texture, clip, &destBuf);
}

void Texture::loadFromText(unsigned int fontSize, string text, SDL_Color c)
{

	m_key = TextureData(text, c);
	m_keyStr = m_key.toString();
	m_value = NULL;
	if (m_container.find(m_keyStr) == m_container.end() || m_container[m_keyStr].lock() == NULL)
	{
		m_value = std::shared_ptr<SDLTexture>(new SDLTexture());
		m_container[m_keyStr] = m_value;
		m_value->loadFromText(fontSize, text, c);
	}
	else
		m_value = m_container[m_keyStr].lock();

	
}

int Texture::getWidth() const
{
	return m_value == NULL ? 0 : m_value->m_w;
}

int Texture::getHeight() const
{
	return m_value == NULL ? 0 : m_value->m_h;
}


Texture::~Texture()
{
	free();
}


