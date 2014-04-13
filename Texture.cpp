#include <iostream>
#include <string>

#include "IDs.h"
#include "WGameCore.h"
#include "Texture.h"

using namespace std;

std::map<std::string, std::weak_ptr<SDLTexture>> Texture::m_container;

Texture::Texture(string id, int r, int g, int b, int a)
{
	load(id, r, g, b, a);
}

Texture::Texture()
{
	load("");
}

void Texture::freeAll()
{
	m_container.clear();
}

void Texture::load(string id, int r, int g, int b, int a)
{
	m_id = id;
	m_instance = NULL;
	if (m_container.find(m_id) == m_container.end() || m_container[m_id].lock() == NULL)
	{
		m_instance = std::shared_ptr<SDLTexture>(new SDLTexture());
		m_container[m_id] = m_instance;
		if (!m_id.empty())
			m_instance->load(id, r, g, b, a);
	}
	else
		m_instance = m_container[id].lock();
		
}

/*
void Texture::copy(Texture const& tex)
{

	free();
	m_w = tex.m_w;
	m_h = tex.m_h;
	m_alpha = tex.m_alpha;
	m_fileName = tex.m_fileName;
	
	SDL_Renderer* renderer = wScreen.getRenderer();
	m_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tex.m_w, tex.m_h);
	SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, m_texture);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, tex.m_texture, NULL, NULL);
	SDL_SetRenderTarget(renderer, NULL);

	//évite les clignotements d'écran (on utilise le même renderer pour copier une texture et pour afficher l'écran, or, on doit clear ce renderer quand on copie une texture,
	//on a donc un clignotement d'écran durant 1 frame lorsqu'on copie une texture. Pour éviter cela, lors de la copie d'une texture on empêche le SDL_RenderPresent de la
	//frame courante).
	//Pour l'instant, je n'ai pas réussi à trouver de manière plus propre.
	wScreen.setNextFrameFlip(false);

}
*/

/*
void Texture::load( string fileName, int r, int g, int b, int a)
{
	

	SDL_Surface *sprite;
	sprite = IMG_Load(fileName.c_str());

	if(sprite == NULL)
 		cerr << "Erreur lors du chargement de l'image \"" << fileName << "\" : " << SDL_GetError() << endl;
	else if(r >= 0 && g >= 0 && b >= 0)
		SDL_SetColorKey(sprite, SDL_TRUE, SDL_MapRGB((sprite)->format, r, g, b));

	//255 : opaque, donc pas besoin d'appliquer de l'alpha
	//si négatif, cela veut dire qu'on ne veut pas d'alpha non plus
	if(a < 255 && a >= 0)
	{
		SDL_SetSurfaceAlphaMod(sprite, a);
		m_alpha = a;
	}

	if(sprite == NULL)
		sprite = IMG_Load(NOSUCHFILE);
	if(sprite == NULL)
		cerr << "Impossible de charger l'image \"" << NOSUCHFILE << "\" : " << SDL_GetError() << endl;

	free();
	m_texture = SDL_CreateTextureFromSurface(wScreen.getRenderer(), sprite);
	m_w = sprite->w;
	m_h = sprite->h;
	m_fileName = fileName;

	SDL_FreeSurface(sprite);
}

void Texture::free()
{
	if(m_texture != NULL)
	{
		SDL_DestroyTexture(m_texture);
		m_texture = NULL;
	}
	m_fileName = NOSUCHFILE;
	m_h = m_w = 0;
	m_alpha = 255;
}
*/

void Texture::setColor( Uint8 r, Uint8 g, Uint8 b)
{
	SDL_SetTextureColorMod(m_instance->m_texture, r, g, b );
}

void Texture::setBlendMode( SDL_BlendMode blending )
{
	SDL_SetTextureBlendMode(m_instance->m_texture, blending);
}

void Texture::setAlpha( Uint8 alpha )
{
	SDL_SetTextureAlphaMod(m_instance->m_texture, alpha);
	m_instance->m_alpha = alpha;
}

int Texture::render( int x, int y, SDL_Rect* clip )
{
	WGameCore& wScreen = WGameCore::getInstance();
	SDL_Rect destBuf = { x, y, m_instance->m_w, m_instance->m_h };

	if( clip != NULL )
	{
		destBuf.w = clip->w;
		destBuf.h = clip->h;
	}


	return SDL_RenderCopy(wScreen.getRenderer(), m_instance->m_texture, clip, &destBuf);
}

void Texture::loadFromText(TTF_Font* font, string text, SDL_Color c)
{
	m_id = text;
	m_instance = NULL;
	if (m_container.find(m_id) == m_container.end() || m_container[m_id].lock() == NULL)
	{
		m_instance = std::shared_ptr<SDLTexture>(new SDLTexture());
		m_container[m_id] = m_instance;
		m_instance->loadFromText(font, text, c);
	}
	else
		m_instance = m_container[m_id].lock();

	
}

void Texture::free()
{
	m_instance = NULL;
	m_id = "";
}

int Texture::getWidth() const
{
	return m_instance->m_w;
}

int Texture::getHeight() const
{
	return m_instance->m_h;
}


Texture::~Texture()
{
	free();
}


